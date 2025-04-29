#pragma once

#include "../KdDirectX.h"

#include "../KdBuffer.h"
#include "../KdTexture.h"

//#include "nlohmann/KdJsonUtility.h"

class KdMaterial;

class KdConstantBufferLayoutInfo;

//============================================
// 
//============================================
class KdResourceInputLayoutInfo
{
public:
	uint32_t		m_bindIdx;
	std::string		m_name;
};

//============================================
// 
// シェーダーパス
// ・簡単に言うと、１種類のシェーダーをまとめたもの
// ・VSやPSやGSを持つ
// ・定数バッファのレイアウト情報
// ・テクスチャなどの入力情報
// 
//============================================
struct KdShaderPass
{
	//=======================================================
	// HLSLファイルをコンパイルし、ファイルとして保存する
	// 保存先のcsoファイルとHLSLファイルの更新日時をチェックし、HLSL更新時のみコンパイルされる
	//=======================================================
	bool LoadShader(
		const std::filesystem::path& baseFolder,	// ベースとなるフォルダ
		const std::filesystem::path& hlslFilename,	// HLSLファイル名
		const std::string& entryName,				// コンパイルする関数名
		const std::string& shaderType,				// シェーダー
		std::vector<D3D_SHADER_MACRO> defines,		// Define
		std::vector<char>& outputCompledBuffer,		// (out)コンパイル済みのシェーダーバッファ
		std::string& errorMsg);						// (out)エラーメッセージ

	//=======================================================
	// コンパイル済みシェーダーファイル(cso)を読み込む
	// outputCompledBuffer … コンパイル済みのシェーダーバイナリが格納される
	//=======================================================
	void ReadCSOFile(
		const std::filesystem::path& filename,			// csoファイル名
		std::vector<char>& outputCompledBuffer);		// コンパイル済みのシェーダーバッファ

	// 定数バッファレイアウト情報取得
	const KdConstantBufferLayoutInfo* GetConstantBufferLayout(uint32_t slotNo) const
	{
		auto it = m_cbLayouts.find(slotNo);
		if (it == m_cbLayouts.end())return nullptr;
		return &it->second;
	}
	// リソース入力情報取得取得
	const KdResourceInputLayoutInfo* GetResourceInputInfo(uint32_t slotNo) const
	{
		auto it = m_resourceInfos.find(slotNo);
		if (it == m_resourceInfos.end())return nullptr;
		return &it->second;
	}

	// コンパイル済みシェーダーバッファから、レイアウト情報を作成する
	void CreateLayoutFromShader(const std::vector<char>& compiledBuffer);

	// 頂点シェーダー作成
	bool CreateVS(const std::vector<char>& compiledBuffer);
	bool CreateInputLayout(const std::vector<char>& compiledBuffer,
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			1,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		2,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		3,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		4,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		}
	);
	// ジオメトリシェーダー作成
	bool CreateGS(const std::vector<char>& compiledBuffer);
	// ピクセルシェーダー作成
	bool CreatePS(const std::vector<char>& compiledBuffer);

	// 
	const std::unordered_map<uint32_t, KdResourceInputLayoutInfo>& GetResouceInputInfos() const { return m_resourceInfos; }

	// 
	KdShaderPass() {}

//private:

	std::string						m_name;						// パス名
	ComPtr<ID3D11VertexShader>		m_VS = nullptr;				// 頂点シェーダー
	ComPtr<ID3D11InputLayout>		m_inputLayout = nullptr;	// 頂点入力レイアウト
	ComPtr<ID3D11PixelShader>		m_PS = nullptr;				// ピクセルシェーダー
	ComPtr<ID3D11GeometryShader>	m_GS = nullptr;				// ジオメトリシェーダー

	// マテリアル定数バッファレイアウト情報
	std::unordered_map<uint32_t, KdConstantBufferLayoutInfo>	m_cbLayouts;

	// リソース入力レイアウト情報
	std::unordered_map<uint32_t, KdResourceInputLayoutInfo>		m_resourceInfos;


	// ロード情報記憶用(リロードで使用)
	struct LoadInfo
	{
		std::string				folderPath;
		std::string				hlslFilename;
		int32_t					passIndex = -1;
		std::string				passName;
		std::string				vsProcName;
		std::string				psProcName;
		std::string				gsProcName;
	};
	LoadInfo	m_loadInfo = {};

private:
	// コピー禁止用
	KdShaderPass(const KdShaderPass& src) = delete;
	void operator=(const KdShaderPass& src) = delete;
};

//============================================
// 
// シェーダー基本クラス
// 
// ・シェーダーは「パス」という単位でグループ化される。
// ・シェーダー内の定数バッファ(Slot0)を、マテリアルのデータとして使用する。
// 
//============================================
class KdShader : public std::enable_shared_from_this<KdShader>
{
public:

	// シェーダー名取得
	const std::string& GetName() const { return m_name; }
	// シェーダー名設定
	void SetName(const std::string& name) { m_name = name; }

	// 指定Indexのパスを取得
	std::shared_ptr<KdShaderPass> GetPass(int32_t passIdx) const
	{
		if (passIdx < 0 || passIdx >= m_passes.size())return nullptr;
		return m_passes[passIdx];
	}

	// パスの登録名からIndexを取得
	int32_t GetPassIndexFromName(const std::string& name) const
	{
		for (int32_t i = 0; i < (int32_t)m_passes.size(); i++)
		{
			if (m_passes[i]->m_name == name)return i;
		}
		return -1;
	}

	// 全パスのリストを取得
	const std::vector<std::shared_ptr<KdShaderPass>>& GetPasses() const { return m_passes; }

	// 指定Indexにシェーダーパスを登録
	void SetPass(uint32_t passIndex, const std::shared_ptr<KdShaderPass>& pass)
	{
		if (passIndex >= (uint32_t)m_passes.size())
		{
			m_passes.resize(passIndex + 1);
		}

		m_passes[passIndex] = pass;
	}

	// シェーダーを作成し、指定Indexのパスとして取得する
	// ・vsProcName		：頂点シェーダーの関数名を指定
	// ・psProcName		：ピクセルシェーダーの関数名を指定
	// ・gsProcName		：ジオメトリシェーダーの関数名を指定
	void LoadShaderAndSetPass(
		const std::string& folderPath, const std::string& hlslFilename,
		uint32_t passIndex, const std::string& passName,
		const std::string& vsProcName, const std::string& psProcName, const std::string& gsProcName = "");

	// 
	void ReloadShader();

	// マテリアル作成
	std::shared_ptr<KdMaterial> CreateMaterial()
	{
		return std::make_shared<KdMaterial>(shared_from_this());
	}

	KdShader() {}

	// 
	~KdShader()
	{
		m_passes.clear();
	}

	// 
	enum PassTags : uint32_t
	{
		Forward = 0,			// 通常描画用パス
		ShadowCaster = 1,		// シャドウマップ描画用
		DepthOnly = 2,			// 深度描画のみ

		// 自由に追加・変更してください
	};

protected:

	// シェーダー名
	std::string m_name;

	// シェーダーパスリスト
	std::vector<std::shared_ptr<KdShaderPass>>			m_passes;

private:
	// コピー禁止用
	KdShader(const KdShader& src) = delete;
	void operator=(const KdShader& src) = delete;
};
