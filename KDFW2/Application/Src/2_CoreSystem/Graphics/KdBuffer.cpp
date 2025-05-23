﻿#include "KdBuffer.h"

#include "KdDirect3D.h"

bool KdBuffer::Create(uint32_t bindFlags, uint32_t bufferSize, D3D11_USAGE bufferUsage, uint32_t structureByteStride, const D3D11_SUBRESOURCE_DATA* initData)
{
	Release();

	if (bufferSize == 0)return false;

	//----------------------------------
	// 定数バッファとして作成する場合はサイズチェックを行う
	// ※定数バッファは16バイトアライメント(16の倍数)にする必要がある
	//----------------------------------
	if (bindFlags == D3D11_BIND_CONSTANT_BUFFER)
	{

		if (bufferSize % 16 != 0) {
			assert(0 && "コンスタントバッファ作成のサイズは16の倍数バイトでなければならないよ!!(Buffer)");
			return false;
		}

		// サイズを16の倍数バイトにする
//		bufferSize = ((bufferSize + 16 - 1) / 16) * 16;

	}

	//--------------------------------
	// バッファ作成のための詳細データ
	//--------------------------------
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags			= bindFlags;		// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	desc.ByteWidth			= bufferSize;		// 作成するバッファのバイトサイズ

	desc.StructureByteStride= structureByteStride;		// 構造化バッファの場合、その構造体のサイズ
	if (structureByteStride >= 1)
	{
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	desc.Usage				= bufferUsage;		// 作成するバッファの使用法

	// 動的ビデオメモリバッファ
	//  GPUからWrite× Read○
	//  CPUからWrite○ Read×
	// 頻繁に更新されるようなバッファはこれが効率良いが、DEFAULTに比べたら少し速度は落ちる
	if (desc.Usage == D3D11_USAGE_DYNAMIC)
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	// 静的ビデオメモリバッファ
	//  GPUからWrite○ Read○
	//  CPUからWrite× Read×　(ただしUpdateSubresource()で更新は可能)
	// ビデオメモリーのバッファ　頻繁に更新するには向いていないが描画が高速
	else if(desc.Usage == D3D11_USAGE_DEFAULT)
	{
		desc.CPUAccessFlags = 0;
	}
	// ステージングバッファ
	//  GPUからWrite× Read×
	//  CPUからWrite○ Read○
	// Direct3Dへバインドは出来ないが、DEFAULTやDYNAMICのバッファに対して読み書き転送が可能
	// (例)DEFAULTバッファの内容を取得したい！ -> STAGINGバッファを作成し、DEFAULTバッファからコピーしてくる(CopyResource関数)。そしてSTAGINGバッファにアクセス(Map/Unmap)する。
	else if (desc.Usage == D3D11_USAGE_STAGING)
	{
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	}
	
	//--------------------------------
	// バッファ作成
	//--------------------------------
	if (FAILED(D3D.GetDev()->CreateBuffer(&desc, initData, &m_pBuffer)))
	{
		assert(0 && "バッファ作成失敗(Buffer)");
		return false;
	}

	// 
	m_bufUsage = bufferUsage;
	m_bufSize = bufferSize;

	// システムメモリ用のバッファも作成しておく
	m_sysMemBuffer.resize(bufferSize);

	if (initData != nullptr)
	{
		memcpy_s(&m_sysMemBuffer[0], m_bufSize, initData->pSysMem, m_bufSize);
	}

	return true;
}


void KdBuffer::WriteData(const void* pSrcData, uint32_t size)
{
	// 動的バッファの場合
	if (m_bufUsage == D3D11_USAGE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE pData;
		// 書き込み専用
		if (SUCCEEDED(D3D.GetDevContext()->Map(m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			memcpy_s(pData.pData, m_bufSize, pSrcData, size);

			// バックアップ用
			memcpy_s(&m_sysMemBuffer[0], m_bufSize, pSrcData, size);

			D3D.GetDevContext()->Unmap(m_pBuffer.Get(), 0);
		}
	}
	// 静的バッファの場合
	else if (m_bufUsage == D3D11_USAGE_DEFAULT)
	{
		// バッファに書き込み
		D3D.GetDevContext()->UpdateSubresource(m_pBuffer.Get(), 0, 0, pSrcData, 0, 0);

		// バックアップ用
		memcpy_s(&m_sysMemBuffer[0], m_bufSize, pSrcData, size);
	}
	// ステージングバッファの場合
	else if (m_bufUsage == D3D11_USAGE_STAGING)
	{
		// 読み書き両方
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(D3D.GetDevContext()->Map(m_pBuffer.Get(), 0, D3D11_MAP_READ_WRITE, 0, &pData)))
		{
			memcpy_s(pData.pData, m_bufSize, pSrcData, size);

			D3D.GetDevContext()->Unmap(m_pBuffer.Get(), 0);
		}
	}

}

void KdBuffer::CopyFrom(const KdBuffer& srcBuffer)
{
	if (m_pBuffer == nullptr)return;
	if (srcBuffer.GetBuffer() == nullptr)return;

	D3D.GetDevContext()->CopyResource(m_pBuffer.Get(), srcBuffer.GetBuffer().Get());
}

void KdConstantBuffer::WriteWorkData(bool forceWrite)
{
	if (forceWrite == false)
	{
		// 変更時のみ書きこむ
		if (m_isDirtyWorkBuffer == false)return;
	}

	if (m_workBuffer.size() == 0)return;

	WriteData(&m_workBuffer[0], (uint32_t)m_workBuffer.size());

	m_isDirtyWorkBuffer = false;
}
