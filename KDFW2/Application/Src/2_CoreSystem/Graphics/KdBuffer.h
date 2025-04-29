#pragma once

class KdDirect3D;


//===============================================================
//
// 様々なバッファを作成・操作するクラス
//
// Direct3D11のID3D11Bufferを簡単に操作できるようにまとめたものです
//  (例)頂点バッファ、インデックスバッファ、定数バッファなど
//
//===============================================================
class KdBuffer {
public:

	//=================================================
	//
	// 取得
	//
	//=================================================
	
	// 作成済み？
	bool					IsCreated() const { return m_pBuffer != nullptr; }

	// バッファインターフェイスを取得
	ComPtr<ID3D11Buffer>	GetBuffer() const { return m_pBuffer; }

	// バッファのサイズを取得
	uint32_t				GetBufferSize() const { return m_bufSize; }

	// 
	std::vector<char>		GetSysMemBuffer() const { return m_sysMemBuffer; }


	//=================================================
	//
	// 作成・解放
	//
	//=================================================

	// バッファを作成
	// ・bindFlags				… どのバッファとしてDirect3Dへバインドするかのフラグ　D3D11_BIND_FLAG定数を指定する
	// ・bufferSize				… 作成するバッファのサイズ(byte)
	// ・bufferUsage			… バッファの使用法　D3D11_USAGE定数を指定する
	// ・structureByteStride	… 構造化バッファにする場合は、１構造体のサイズを指定する
	// ・initData				… 作成時に書き込むデータ nullptrだと何も書き込まない
	bool Create(uint32_t bindFlags, uint32_t bufferSize, D3D11_USAGE bufferUsage, uint32_t structureByteStride, const D3D11_SUBRESOURCE_DATA* initData);

	// 定数バッファとして作成
	// ・bufferSize				… 作成するバッファのサイズ(byte)
	// ・initData				… 初期に流し込むデータ
	bool CreateConstantBuffer(uint32_t bufferSize, const void* initData = nullptr)
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = initData;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;
		// 作成
		return Create(D3D11_BIND_CONSTANT_BUFFER, bufferSize, D3D11_USAGE_DYNAMIC, 0, initData != nullptr ? &srd : nullptr);
	}

	// ストリームアウトプットバッファの作成
	bool CreateStreamOutputBuffer(uint32_t bufferSize, const void* initData = nullptr)
	{
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = initData;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;

		// 作成
		return Create(	D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
						bufferSize, D3D11_USAGE_DEFAULT, 0, initData != nullptr ? &srd : nullptr);
	}

	// 解放
	void Release()
	{
		m_pBuffer.Reset();
		m_bufSize = 0;
		m_bufUsage = D3D11_USAGE_DEFAULT;

		m_sysMemBuffer.clear();
	}

	//=================================================
	//
	// 操作
	//
	//=================================================

	// バッファへ指定データを書き込み
	// Dynamicの場合はD3D11_MAP_WRITE_DISCARDでMap。
	// Defaultの場合はUpdateSubResource。
	// Stagingの場合はD3D11_MAP_READ_WRITEでMap。
	// ・pSrcData		… 書き込みたいデータの先頭アドレス
	// ・size			… 書き込むサイズ(byte)
	void WriteData(const void* pSrcData, uint32_t size);


	// GPU上でバッファのコピーを実行する
	// ※詳細はDeviceContextのCopyResource()参照 https://msdn.microsoft.com/ja-jp/library/ee419574(v=vs.85).aspx
	//  一部抜粋
	//  ※異なるリソースである必要があります。
	//  ※同じ種類である必要があります。
	//  ※次元 (幅、高さ、深度、サイズなど) が同じである必要があります。
	//  ※単純なコピーのみが行われます。CopyResource では、引き伸ばし、カラー キー、ブレンド、フォーマット変換はサポートされません。
	//  ※DXGIフォーマットの互換性が必要です。
	// ・srcBuffer		… コピー元バッファ
	void CopyFrom(const KdBuffer& srcBuffer);

	//=================================================

	//
	KdBuffer() {}
	// 
	~KdBuffer() {
		Release();
	}

protected:

	// バッファ本体
	ComPtr<ID3D11Buffer>	m_pBuffer = nullptr;

	std::vector<char>		m_sysMemBuffer;

	// バッファのサイズ(byte)
	uint32_t				m_bufSize = 0;

	// バッファの使用法
	D3D11_USAGE				m_bufUsage = D3D11_USAGE_DEFAULT;

private:
	// コピー禁止用
	KdBuffer(const KdBuffer& src) = delete;
	void operator=(const KdBuffer& src) = delete;
};

//============================================
// 
// 定数バッファのレイアウト情報
// 
//============================================
class KdConstantBufferLayoutInfo
{
public:
	// データの型
	enum Types
	{
		Unknown,
		Int,		// Int型
		Float,		// Float型
		// 型はもっとあるよ
	};

	// １変数の情報
	struct VariableData
	{
		std::string Name;			// 変数名
		uint32_t	StartOffset;	// 開始バイト位置
		uint32_t	ByteSize;		// 変数のバイト数

		Types		Type;			// データ型
		uint8_t		Rows;			// 行数	例)行列用
		uint8_t		Columns;		// 桁数 例)行列は1 3Dベクトルだと3
	};


	// 定数バッファサイズ(Byte)取得
	uint32_t GetBufferSize() const { return m_bufferSize; }
	// 定数バッファサイズ(Byte)設定
	void SetBufferSize(uint32_t size) { m_bufferSize = size; }

	void SetVariableData(const VariableData& data)
	{
		m_variableMaps[data.Name] = data;
	}


	// 変数の情報を取得
	const VariableData* GetVariableInfo(const std::string& name) const
	{
		auto it = m_variableMaps.find(name);
		if (it == m_variableMaps.end()) return nullptr;

		return &it->second;
	}


	const std::unordered_map<std::string, VariableData>& GetVariableMaps() const { return m_variableMaps; }

private:

	// 定数バッファのサイズ
	uint32_t m_bufferSize = 0;
	// 定数バッファの変数レイアウト情報
	std::unordered_map<std::string, VariableData> m_variableMaps;
};

//===============================================================
// 
// 定数バッファとして便利な機能があるバッファクラス
// 
//===============================================================
class KdConstantBuffer : public KdBuffer
{
public:

	// 作業領域取得　※変更フラグがONになります
	std::vector<char>& EditWorkBuffer()
	{
		m_isDirtyWorkBuffer = true;	// 変更フラグON
		return m_workBuffer;
	}

	// 作業領域取得　※読み取り専用　変更フラグは変化しません
	const std::vector<char>& GetWorkBuffer() const { return m_workBuffer; }

	// 定数バッファ変数レイアウトデータ
	const KdConstantBufferLayoutInfo& GetLayoutInfo() const{ return m_layout; }


	// 定数バッファを作成
	// ・定数バッファレイアウトデータを使用
	bool CreateConstantBuffer(KdConstantBufferLayoutInfo cbLayout, const void* initData = nullptr)
	{
		m_layout = cbLayout;

		return CreateConstantBuffer(m_layout.GetBufferSize(), initData);
	}

	bool CreateConstantBuffer(uint32_t bufferSize, const void* initData = nullptr)
	{
		// サイズを16の倍数バイトにする
		bufferSize = ((bufferSize + 16 - 1) / 16) * 16;

//		m_layout = {};
		m_layout.SetBufferSize(bufferSize);

		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = initData;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;

		// 作成
		if (Create(D3D11_BIND_CONSTANT_BUFFER, m_layout.GetBufferSize(), D3D11_USAGE_DYNAMIC, 0, initData != nullptr ? &srd : nullptr))
		{
			// 作業用バッファを作成
			m_workBuffer.resize(bufferSize);
			if (initData != nullptr)
			{
				// 初期値を書き込む
				memcpy_s(&m_workBuffer[0], m_workBuffer.size(), initData, m_workBuffer.size());
			}

			return true;
		}
		return false;
	}

	// バッファへWorkBufferのデータを描きこむ
	void WriteWorkData(bool forceWrite = false);

	void SetVS(uint32_t slotNo)
	{
		D3D.GetDevContext()->VSSetConstantBuffers(slotNo, 1, m_pBuffer.GetAddressOf());
	}
	void SetPS(uint32_t slotNo)
	{
		D3D.GetDevContext()->PSSetConstantBuffers(slotNo, 1, m_pBuffer.GetAddressOf());
	}
	void SetGS(uint32_t slotNo)
	{
		D3D.GetDevContext()->GSSetConstantBuffers(slotNo, 1, m_pBuffer.GetAddressOf());
	}

	//---------------------------------
	// 定数バッファへのデータ書き込み
	//---------------------------------

	// 値をセット
	template<class T>
	void SetValue(const std::string& name, const T& data)
	{
		auto variableInfo = m_layout.GetVariableInfo(name);
		if (variableInfo == nullptr)return;

		memcpy_s(&EditWorkBuffer()[variableInfo->StartOffset],
			variableInfo->ByteSize,
			&data,
			sizeof(T));
	}

	// 値を取得
	template<class T>
	void GetValue(const std::string& name, T& retValue) const
	{
		auto variableInfo = m_layout.GetVariableInfo(name);
		if (variableInfo == nullptr)return;

		memcpy_s(&retValue,
			sizeof(T),
			&GetWorkBuffer()[variableInfo->StartOffset],
			variableInfo->ByteSize);
	}

protected:

	// 定数バッファの変数情報
	KdConstantBufferLayoutInfo m_layout = {};

	// 作業用バッファ
	std::vector<char>		m_workBuffer;
	bool					m_isDirtyWorkBuffer = true;
};

//===============================================================
// 
//===============================================================
template<class StructType>
class KdConstantBufferType : public KdConstantBuffer
{
public:

	// 定数バッファの作業バッファを取得　※変更フラグがONになります
	StructType& EditCB()
	{
		m_isDirtyWorkBuffer = true;
		return (StructType&)m_workBuffer[0];
	}

	bool CreateConstantBuffer()
	{
		StructType data = {};

		return KdConstantBuffer::CreateConstantBuffer((uint32_t)sizeof(StructType), &data);
	}

};

//========================================================
// 
// 定数バッファクラス
// 
//========================================================
/*
class KdConstantBuffer : public KdBuffer
{
	// DataType型のサイズの定数バッファを作成
	// ・initData		… 作成時にバッファに書き込むデータ　nullptrで何も書き込まない
	bool CreateConstantBuffer(uint32_t bufferSize, const void* initData = nullptr)
	{
		Release();

		m_bufUsage = D3D11_USAGE_DYNAMIC;

		//----------------------------------
		// バッファ作成
		//----------------------------------
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = initData;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;
		// 作成
		return Create(D3D11_BIND_CONSTANT_BUFFER, bufferSize, m_bufUsage, 0, &srd);
	}

};
*/

/*
//========================================================
//
// 作業データ付き 定数バッファクラス
//
//  Bufferに、バッファと同じサイズの作業データを持たせたり、
//  更新した時だけバッファに書き込みを行ったりと、管理を楽にしたクラス
//
//========================================================
template<class DataType>
class KdConstantBuffer {
public:

	//=================================================
	//
	// 取得・設定
	//
	//=================================================

	// 作業領域取得　※変更フラグがONになります
	DataType& Work()
	{
		m_isDirty = true;	// 変更フラグON
		return m_work;
	}

	// 作業領域取得　※読み取り専用　変更フラグは変化しません
	const DataType&			GetWork() const { return m_work; }

	// バッファアドレス取得
	ComPtr<ID3D11Buffer>	GetBuffer() const { return m_buffer.GetBuffer(); }


	// m_workを定数バッファへ書き込む
	// ※m_isDirtyがtrueの時のみ、バッファに書き込まれる
	void Write()
	{
		// 変更がある時だけ書き込む
		if (m_isDirty)
		{
			m_buffer.WriteData(&m_work, m_buffer.GetBufferSize());
			m_isDirty = false;
		}
	}


	//=================================================
	//
	// 作成・解放
	//
	//=================================================

	// DataType型のサイズの定数バッファを作成
	// ・initData		… 作成時にバッファに書き込むデータ　nullptrで何も書き込まない
	bool Create(const DataType* initData = nullptr)
	{
		Release();

		if (initData)
		{
			m_work = *initData;
		}

		//----------------------------------
		// バッファ作成
		//----------------------------------
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = &m_work;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;
		// 作成
		return m_buffer.Create(	D3D11_BIND_CONSTANT_BUFFER, sizeof(DataType), D3D11_USAGE_DYNAMIC, 0, &srd);
	}

	// 解放
	void Release()
	{
		m_buffer.Release();
		m_isDirty = true;
	}

	// 
	~KdConstantBuffer()
	{
		Release();
	}

	KdConstantBuffer() = default;

private:

	// 定数バッファ
	KdBuffer			m_buffer;

	// 作業用 定数バッファ
	// この内容がWrite関数で定数バッファ本体に方に書き込まれる
	DataType			m_work;

	// データ更新フラグ　パフォーメンス向上のため、これがtrueの時だけWrite()でデータ書き込み実行されるようにしています
	bool				m_isDirty = true;

private:
	// コピー禁止用
	KdConstantBuffer(const KdConstantBuffer& src) = delete;
	void operator=(const KdConstantBuffer& src) = delete;
};
*/

/*
//========================================================
//
// 作業データ付き 定数バッファクラス
//
//  Bufferに、バッファと同じサイズの作業データを持たせたり、
//  更新した時だけバッファに書き込みを行ったりと、管理を楽にしたクラス
//
//========================================================
template<class DataType>
class KdConstantBuffer : public KdBuffer
{
public:

	//=================================================
	//
	// 取得・設定
	//
	//=================================================

	// 作業領域取得　※変更フラグがONになります
	DataType& EditData()
	{
		m_isDirty = true;	// 変更フラグON
		return m_work;
	}

	// 作業領域取得　※読み取り専用　変更フラグは変化しません
	const DataType& GetData() const { return m_work; }

	// バッファアドレス取得
//	ComPtr<ID3D11Buffer>	GetBuffer() const { return m_buffer.GetBuffer(); }


	// m_workを定数バッファへ書き込む
	// ※m_isDirtyがtrueの時のみ、バッファに書き込まれる
	void Write()
	{
		// 変更がある時だけ書き込む
		if (m_isDirty)
		{
			WriteData(&m_work, GetBufferSize());
			m_isDirty = false;
		}
	}


	//=================================================
	//
	// 作成・解放
	//
	//=================================================

	// DataType型のサイズの定数バッファを作成
	// ・initData		… 作成時にバッファに書き込むデータ　nullptrで何も書き込まない
	bool CreateConstantBuffer(const DataType* initData = nullptr)
	{
		Release();

		if (initData)
		{
			m_work = *initData;
		}

		//----------------------------------
		// バッファ作成
		//----------------------------------
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = &m_work;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;
		// 作成
		return Create(D3D11_BIND_CONSTANT_BUFFER, sizeof(DataType), D3D11_USAGE_DYNAMIC, 0, &srd);
	}

	// 解放
	void Release()
	{
		Release();
		m_isDirty = true;
	}

	// 
	~KdConstantBuffer()
	{
		Release();
	}

	KdConstantBuffer() = default;

private:

	// 作業用 定数バッファ
	// この内容がWrite関数で定数バッファ本体に方に書き込まれる
	DataType			m_work;

	// データ更新フラグ　パフォーメンス向上のため、これがtrueの時だけWrite()でデータ書き込み実行されるようにしています
	bool				m_isDirty = true;

private:
	// コピー禁止用
	KdConstantBuffer(const KdConstantBuffer& src) = delete;
	void operator=(const KdConstantBuffer& src) = delete;
};
*/

//========================================================
//
// 構造化バッファクラス
//
//  指定型の作業用バッファを配列で持っていたり、
//  更新した時だけバッファに書き込みを行ったりと、管理を楽にしたクラス
//
//========================================================
template<class DataType>
class KdStructuredBuffer
{
public:

	//=================================================
	//
	// 取得・設定
	//
	//=================================================

	// 作業領域取得　※変更フラグがONになります
	std::vector<DataType>& Work()
	{
		m_isDirty = true;	// 変更フラグON
		return m_work;
	}

	// バッファアドレス取得
	ComPtr<ID3D11Buffer>	GetBuffer() const { return m_buffer.GetBuffer(); }

	// 作業領域配列取得　※読み取り専用　変更フラグは変化しません
	const std::vector<DataType>& GetWork() const { return m_work; }

	// シェーダリソースビュー取得
	ComPtr<ID3D11ShaderResourceView> GetSRView() const { return m_srv; }

	// アンオーダードアクセスビュー取得
	ComPtr<ID3D11UnorderedAccessView> GetUAView() const { return m_uav; }

	// m_workを定数バッファへ書き込む
	// ※m_isDirtyがtrueの時のみ、バッファに書き込まれる
	void Write()
	{
		if (m_buffer.IsCreated() == false)return;

		// 変更がある時だけ書き込む
		if (m_isDirty)
		{
			m_buffer.WriteData(&m_work[0], m_buffer.GetBufferSize());
			m_isDirty = false;
		}
	}

	//=================================================
	//
	// 作成・解放
	//
	//=================================================

	// DataType型のサイズの構造化バッファを作成
	// ・numDatas			… 配列の個数
	// ・isUnorderedAccess	… UnorderedAcccessとして作成。
	// ・initData			… 作成時にバッファに書き込むデータ　nullptrで何も書き込まない
	bool Create(int numDatas, bool isUnorderedAccess = false, const DataType* initData = nullptr)
	{
		Release();
		if (numDatas <= 0)return false;

		UINT bufferSize = sizeof(DataType) * numDatas;

		//----------------------------------
		// 作業データ確保
		//----------------------------------
		m_work.resize(numDatas);
		// 初期データコピー
		if (initData)
		{
			memcpy_s(&m_work[0], bufferSize, initData, bufferSize);
		}

		//----------------------------------
		// バッファ作成
		//----------------------------------

		UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
		D3D11_USAGE usage = D3D11_USAGE_DYNAMIC;

		if (isUnorderedAccess)
		{
			bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
			usage = D3D11_USAGE_DEFAULT;
		}

		// 初期で書き込むデータ
		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = &m_work[0];
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;
		// バッファ作成
		if (m_buffer.Create(bindFlags, bufferSize, usage, sizeof(DataType), &srd) == false)
		{
			Release();
			return false;
		}

		//----------------------------------
		// SRV作成
		//----------------------------------
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = numDatas;

			if (FAILED(KdDirect3D::GetInstance().GetDev()->CreateShaderResourceView(m_buffer.GetBuffer(), &desc, &m_srv)))
			{
				Release();
				return false;
			}
		}

		//----------------------------------
		// UAV作成
		//----------------------------------
		if (isUnorderedAccess)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			desc.Buffer.NumElements = numDatas;

			if (FAILED(KdDirect3D::GetInstance().GetDev()->CreateUnorderedAccessView(m_buffer.GetBuffer(), &desc, &m_uav)))
			{
				Release();
				return false;
			}
		}

		return true;
	}

	// 解放
	void Release()
	{
		m_buffer.Release();
		m_srv.Reset();
		m_uav.Reset();
	}

	// 
	~KdStructuredBuffer()
	{
		Release();
	}


	KdStructuredBuffer() = default;

private:

	// 定数バッファ
	KdBuffer					m_buffer;
	// 作業データ
	std::vector<DataType>		m_work;

	// ビュー
	ComPtr<ID3D11ShaderResourceView>	m_srv = nullptr;
	ComPtr<ID3D11UnorderedAccessView>	m_uav = nullptr;

	// データ更新フラグ　パフォーメンス向上のため、これがtrueの時だけWrite()でデータ書き込み実行されるようにしています
	bool						m_isDirty = true;

private:
	// コピー禁止用
	KdStructuredBuffer(const KdStructuredBuffer& src) = delete;
	void operator=(const KdStructuredBuffer& src) = delete;
};
