#pragma once

#include "KdShader.h"

//============================================
// 
// マテリアル
// 
//============================================
class KdMaterial
{
public:

	// １パスの情報
	// ・シェーダー(VS,PS,GSなど)、定数バッファ、テクスチャ
	struct Pass
	{
		bool									m_opaque = true;		// 不透明

		// パスIndex
		int32_t									m_passIndex = -1;

		// マテリアルデータ用 定数バッファ
		std::shared_ptr<KdConstantBuffer>		m_cBuffer;

		// テクスチャ
		std::unordered_map<uint32_t, std::shared_ptr<KdTexture>>	m_textures;
	};

	// シェーダーから作成
	KdMaterial(const std::shared_ptr<KdShader>& shader)
	{
		m_shader = shader;

		// m_shaderからパスを作成する
		CreatePassesFromShader(nullptr);
	}

	// 他のマテリアルから作成
	KdMaterial(KdMaterial& material)
	{
		m_name = material.m_name;
		m_shader = material.m_shader;

		// m_shaderからパスを作成する
		CreatePassesFromShader(&material);
	}

	// 複製を作成し返す
	std::shared_ptr<KdMaterial> Clone()
	{
		auto mate = std::make_shared<KdMaterial>(*this);
		mate->m_instanced = true;
		return mate;
	}

	// シェーダー取得
	std::shared_ptr<KdShader> GetShader() const { return m_shader; }

	// マテリアル名
	const std::string& GetName() const { return m_name; }

	// パス名から、Index取得
	int32_t GetPassIndexFromName(const std::string& name) const { return m_shader->GetPassIndexFromName(name); }

	// データをセット
	void SetData(const std::string& name);

	// 不透明？
	bool IsOpaque() const { return true; }

	// コピー品？
	bool IsInstanced() const { return m_instanced; }

	// マテリアルの定数バッファに値をセット
	// ・passIdx … パス番号
	// ・name … セットする変数名
	// ・data … セットするデータ
	template<class T>
	void SetValue(uint32_t passIdx, const std::string& name, const T& data)
	{
		Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return;
		if (pass->m_cBuffer == nullptr)return;

		pass->m_cBuffer->SetValue(name, data);
	}

	// マテリアルの定数バッファから、値を取得
	// ・passIdx … パス番号
	// ・name … セットする変数名
	// ・retValue … 取得したデータを格納する変数
	template<class T>
	void GetValue(uint32_t passIdx, const std::string& name, T& retValue) const
	{
		const Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return;
		if (pass->m_cBuffer == nullptr)return;

		pass->m_cBuffer->GetValue(name, retValue);
	}

	// マテリアルデータやシェーダーをデバイスへセットする
	bool SetToDevice(uint32_t passIdx);

	bool IsValidVS(uint32_t passIdx) const
	{
		const Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return false;

		auto shaderPass = m_shader->GetPass(pass->m_passIndex);
		return shaderPass->m_VS != nullptr;
	}
	
	bool IsValidPS(uint32_t passIdx) const
	{
		const Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return false;

		auto shaderPass = m_shader->GetPass(pass->m_passIndex);
		return shaderPass->m_PS != nullptr;
	}

	bool IsValidGS(uint32_t passIdx) const
	{
		const Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return false;

		auto shaderPass = m_shader->GetPass(pass->m_passIndex);
		return shaderPass->m_GS != nullptr;
	}

	// テクスチャをセット
	void SetTexture(uint32_t passIdx, uint32_t bindIdx, const std::shared_ptr<KdTexture>& tex)
	{
		Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return;

		pass->m_textures[bindIdx] = tex;
	}

	Pass* GetPass(uint32_t passIdx)
	{
		if (passIdx >= m_passes.size())return nullptr;
		return &m_passes[passIdx];
	}
	const Pass* GetPass(uint32_t passIdx) const
	{
		if (passIdx >= m_passes.size())return nullptr;
		return &m_passes[passIdx];
	}

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const nlohmann::json& jsonData);

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(nlohmann::json& outJson) const;

private:

	// m_shaderからパスを作成する
	// ・srcMaterial：デートを引き継ぐ元マテリアル。nullptrで引き継がない。
	void CreatePassesFromShader(KdMaterial* srcMaterial)
	{
		m_passes.clear();

		// 全シェーダーパスから、マテリアル作成
		for (int i = 0; i < m_shader->GetPasses().size(); i++)
		{
			auto srcPass = m_shader->GetPasses()[i];
			if (srcPass == nullptr)continue;

			Pass pass;
			pass.m_passIndex = i;

			// シェーダーの定数バッファ情報
			auto cbIndo = srcPass->GetConstantBufferLayout(0);

			// 定数バッファ作成
			if (cbIndo != nullptr)
			{
				pass.m_cBuffer = std::make_shared<KdConstantBuffer>();

				if (srcMaterial == nullptr)
				{
					pass.m_cBuffer->CreateConstantBuffer(*cbIndo);
				}
				else
				{
					pass.m_cBuffer->CreateConstantBuffer(*cbIndo, &srcMaterial->GetPass(i)->m_cBuffer->GetWorkBuffer()[0]);

					// テクスチャ
					pass.m_textures = srcMaterial->GetPass(i)->m_textures;
				}
			}

			m_passes.push_back(pass);
		}
	}

private:

	// マテリアル名
	std::string						m_name;

	// シェーダー
	std::shared_ptr<KdShader>		m_shader;

	// シェーダーパス単位のデータ
	std::vector<Pass>				m_passes;


	// コピー品か？
	bool							m_instanced = false;

private:
	// コピー禁止用
	KdMaterial(const KdMaterial& src) = delete;
	void operator=(const KdMaterial& src) = delete;
};
