#include "KdShader.h"

static const char* g_includePath = "Src\\2_CoreSystem\\Graphics\\Shader";

// file1とfile2の更新日時をチェックし、file1の方が新しい場合はtrue
static bool CompareFileTime(const std::filesystem::path& file1, const std::filesystem::path& file2)
{
	std::filesystem::file_time_type time1 = std::filesystem::last_write_time(file1);
	std::filesystem::file_time_type time2 = std::filesystem::last_write_time(file2);

	if (time1 > time2)return true;

	return false;
}

//========================================================
// 
// HLSLソース解析時の#includeを解決するためのクラス
// 
//========================================================
class ShaderIncludeClass : public ID3DInclude
{
public:
	// インクルードファイルのディレクトリリスト
	// #includeは、ここに登録されているフォルダで検索されるようにするため、事前にパスを入れておくこと
	std::vector<std::filesystem::path> m_dirs;

	// 解析後、インクルードしたヘッダーファイルのパスリスト
	std::vector<std::filesystem::path> m_headerFilePathList;

	// 読み込んだヘッダーファイルの内容
	std::string m_buffer;

	// ヘッダーファイルのパスがきた時
	HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
	{
		for (auto&& dir : m_dirs)
		{
			// 指定したヘッダーファイルが、このフォルダに存在する？
			std::filesystem::path headerFilePath = dir / pFileName;

			std::ifstream file(headerFilePath);
			if (file)
			{
				// 存在する場合、そのパスを記憶しておく
				m_headerFilePathList.push_back(headerFilePath);

				// ヘッダーファイルの中身をソースに追加するため、ファイルを読み込む
				m_buffer = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());	// ファイル読み込み

				// このヘッダーファイルの内容が、HLSLソースコードに追加されるように指示
				*ppData = &m_buffer[0];
				*pBytes = (uint32_t)m_buffer.size();

				return S_OK;
			}
		}

		return S_FALSE;
	}
	HRESULT Close(LPCVOID pData) override
	{
		m_buffer = "";
		return S_OK;
	}
};

// HLSLファイルをコンパイルし、ファイルとして保存する
bool KdShaderPass::LoadShader(
	const std::filesystem::path& baseFolder,
	const std::filesystem::path& hlslFilename,
	const std::string& entryName,
	const std::string& shaderType,
	std::vector<D3D_SHADER_MACRO> defines,
	std::vector<char>& outputCompledBuffer,
	std::string& errorMsg)
{
	// define関係
	// 終端を追加
	defines.push_back({ nullptr, nullptr });

	// ファイルパス
	std::filesystem::path HLSLfullpath = baseFolder / hlslFilename;

	// csoファイルのパス(コンパイル済みシェーダー)
	std::string outpuFullFilename = (baseFolder / hlslFilename.stem()).generic_string() + "_" + entryName + ".cso";

	bool existCSOFile = std::filesystem::exists(outpuFullFilename);
	bool existHLSLFile = std::filesystem::exists(HLSLfullpath);
	// ファイルが存在しない
	if (existCSOFile == false && existHLSLFile == false)
	{
		outputCompledBuffer.clear();
		errorMsg = "ファイルが存在しない";
		return false;
	}

	// CSOのみ存在の場合は、それを読み込んで終了
	if (existCSOFile && existHLSLFile == false)
	{
		ReadCSOFile(outpuFullFilename, outputCompledBuffer);

		errorMsg = "";
		return true;
	}

	//★★★★★★★★★★★★★★★★★
	// 
	// HLSLファイル前処理実行
	// 
	// ※前処理とは？
	// ・HLSLファイルを解析し、ソース内で記述されている#includeなどの情報を取得。
	// ・#defineと#ifdefで、不要な部分があれば削除する。
	// ・HLSL内の#includeを全て結合し、１つのソースコードにする。
	// 
	//★★★★★★★★★★★★★★★★★

	// 前処理済みのHLSLソースコードを格納する場所
	ComPtr<ID3DBlob> preprocessBlob = nullptr;

	{
		// 追加のヘッダーフォルダパス
		ShaderIncludeClass inc;
		inc.m_dirs.push_back(baseFolder);
		inc.m_dirs.push_back(g_includePath);	// 共通のファイルが置いているフォルダも対象にする

		// HLSLソースファイルをそのまま読み込む
		std::ifstream file(HLSLfullpath.generic_wstring().c_str(), std::ios::binary);
		auto hlslFileData = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		// 読み込んだHLSLソースファイルを前処理する
		// (defineやincludeを考慮し、１つのソースファイルに結合する)
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3DPreprocess(&hlslFileData[0], hlslFileData.size(), nullptr, &defines[0], &inc, &preprocessBlob, &errorBlob);
		if (FAILED(hr))
		{
			// 失敗時
			if (errorBlob)
			{
				errorMsg = (char*)errorBlob->GetBufferPointer();
			}

			return false;
		}

		// HLSLとCSO 両方存在する場合は、更新日時をチェック
		if (existCSOFile && existHLSLFile)
		{
			//---------------------------------------------------
			// CSOファイルと比べて、
			// HLSLファイルや各ヘッダーファイルのどれかに更新があったか？
			//---------------------------------------------------

			bool neesCompile = false;

			// includeされた全ヘッダーファイル
			for (auto&& headerFilePath : inc.m_headerFilePathList)
			{
				// ヘッダーファイルと、CSOファイルの更新日時チェック
				// ヘッダーの方が新しいなら、再コンパイルが必要！
				if (CompareFileTime(headerFilePath, outpuFullFilename)) neesCompile = true;
			}

			// HLSLソースファイルと、CSOファイルの更新日時チェック
			// HLSLソースファイルの方が新しいなら、再コンパイルが必要！
			if (CompareFileTime(HLSLfullpath, outpuFullFilename)) neesCompile = true;

			// 再コンパイルが必要ないなら、CSOをそのまま読み込んで終了
			if (neesCompile == false)
			{
				ReadCSOFile(outpuFullFilename, outputCompledBuffer);

				errorMsg = "";
				return true;
			}
		}
	}


	//★★★★★★★★★★★★★★★★★
	// 
	// HLSLシェーダーコードのコンパイル
	// 
	//★★★★★★★★★★★★★★★★★

	{
		// コンパイルフラグ
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;// | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		// コンパイルされたシェーダーバイナリを格納する場所
		ComPtr<ID3DBlob> shaderBlob = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;

		// 前処理されたシェーダーコードをコンパイル
		HRESULT hr = D3DCompile(preprocessBlob->GetBufferPointer(), preprocessBlob->GetBufferSize(), nullptr, nullptr, nullptr,
			entryName.c_str(), shaderType.c_str(),
			flags, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			// 失敗時
			if (errorBlob)
			{
				errorMsg = (char*)errorBlob->GetBufferPointer();
			}

			return false;
		}

		// コンパイルされたシェーダーバイナリを返す(コピーする)
		outputCompledBuffer.resize(shaderBlob->GetBufferSize());
		memcpy_s(outputCompledBuffer.data(), shaderBlob->GetBufferSize(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
	}

	//★★★★★★★★★★★★★★★★★
	// 
	// シェーダー内容の解析
	// 
	// ・定数バッファの情報や、テクスチャ入力の情報など、シェーダー内の詳細を知ることが出来る
	// 
	//★★★★★★★★★★★★★★★★★
	if (outputCompledBuffer.size() > 0)
	{
		// コンパイル済みシェーダーから、詳細情報を取得する
		CreateLayoutFromShader(outputCompledBuffer);
	}

	//★★★★★★★★★★★★★★★★★
	// 
	// CSOファイルファイル保存
	// 
	//★★★★★★★★★★★★★★★★★
	// コンパイル済みシェーダーを、csoファイルファイルに保存する
	std::ofstream ofs(outpuFullFilename, std::ios::binary);
	if (ofs)
	{
		ofs.write(&outputCompledBuffer[0], outputCompledBuffer.size());
	}

	errorMsg = "";
	return true;
}

void KdShaderPass::ReadCSOFile(const std::filesystem::path& filename, std::vector<char>& outputCompledBuffer)
{
	outputCompledBuffer.clear();

	//------------------------
	// コンパイル済みシェーダーファイルの読み込み
	//------------------------
	std::ifstream file(filename, std::ios::binary);
	if (!file) return;

	// ファイル読み込み
	outputCompledBuffer = std::vector(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

	//------------------------
	// コンパイル済みシェーダーから、
	// 詳細情報を取得する
	//------------------------
	if (outputCompledBuffer.size() > 0)
	{
		CreateLayoutFromShader(outputCompledBuffer);
	}
}


void KdShaderPass::CreateLayoutFromShader(const std::vector<char>& compiledBuffer)
{
	//==========================================
	// 
	// コンパイル済みシェーダーから、詳細情報を取得する
	// 
	//==========================================
	ComPtr<ID3D11ShaderReflection> reflection;
	if (SUCCEEDED(D3DReflect(&compiledBuffer[0], compiledBuffer.size(), IID_ID3D11ShaderReflection, (void**)&reflection)))
	{
		// シェーダーの情報
		D3D11_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);

		// リソースのバインド情報
		for (uint32_t iBind = 0; iBind < shaderDesc.BoundResources; iBind++)
		{
			// バインド情報
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(iBind, &bindDesc);

			// 定数バッファかつ0番目にバインドされているヤツ
			if (bindDesc.Type == D3D_SIT_CBUFFER)
			{
				KdConstantBufferLayoutInfo cbInfo;

				// 定数バッファ名から情報取得
				auto pCB = reflection->GetConstantBufferByName(bindDesc.Name);
				D3D11_SHADER_BUFFER_DESC cbDesc;
				pCB->GetDesc(&cbDesc);

				// 定数バッファのサイズを記憶しておく
				cbInfo.SetBufferSize(cbDesc.Size);

				// 定数バッファ内の情報
				for (uint32_t i = 0; i < cbDesc.Variables; i++)
				{
					auto pVariable = pCB->GetVariableByIndex(i);
					D3D11_SHADER_VARIABLE_DESC variDesc;
					pVariable->GetDesc(&variDesc);

					// 型
					D3D11_SHADER_TYPE_DESC typeDesc;
					pVariable->GetType()->GetDesc(&typeDesc);

					// 情報を覚える
					KdConstantBufferLayoutInfo::VariableData data;
					data.Name = variDesc.Name;
					data.StartOffset = variDesc.StartOffset;
					data.ByteSize = variDesc.Size;

					// 型
					switch (typeDesc.Type)
					{
					case D3D_SHADER_VARIABLE_TYPE::D3D10_SVT_FLOAT:
						data.Type = KdConstantBufferLayoutInfo::Float;
						break;
					case D3D_SHADER_VARIABLE_TYPE::D3D10_SVT_INT:
						data.Type = KdConstantBufferLayoutInfo::Int;
						break;
					default:
						data.Type = KdConstantBufferLayoutInfo::Float;
						break;
					};

					// 行数と桁数
					data.Rows = typeDesc.Rows;
					data.Columns = typeDesc.Columns;

					cbInfo.SetVariableData(data);
				}

				// 登録
				m_cbLayouts[bindDesc.BindPoint] = cbInfo;
			}
			else if (bindDesc.Type == D3D_SIT_TEXTURE)
			{
				D3D11_SHADER_INPUT_BIND_DESC sibDesc;
				reflection->GetResourceBindingDescByName(bindDesc.Name, &sibDesc);

				KdResourceInputLayoutInfo info = {};
				info.m_bindIdx = sibDesc.BindPoint;
				info.m_name = sibDesc.Name;

				// 登録
				m_resourceInfos[sibDesc.BindPoint] = info;
			}
		}
	}
}

bool KdShaderPass::CreateVS(const std::vector<char>& compiledBuffer)
{
	// シェーダー作成
	if (FAILED(D3D.GetDev()->CreateVertexShader(&compiledBuffer[0], compiledBuffer.size(), nullptr, &m_VS)))
	{
		assert(0 && "頂点シェーダー作成失敗");
		return false;
	}
	return true;
}

bool KdShaderPass::CreateInputLayout(const std::vector<char>& compiledBuffer, std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout)
{
	// １頂点の詳細な情報
	/*
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			1,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		2,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		3,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		4,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	*/

	// 頂点入力レイアウト作成
	if (FAILED(D3D.GetDev()->CreateInputLayout(
		&inputLayout[0],								// 入力エレメント先頭アドレス
		(uint32_t)inputLayout.size(),				// 入力エレメント数
		&compiledBuffer[0],						// 頂点バッファのバイナリデータ
		(uint32_t)compiledBuffer.size(),		// 上記のバッファサイズ
		&m_inputLayout))					// 
		)
	{
		assert(0 && "CreateInputLayout作成失敗");
		return false;
	}

	return true;
}

bool KdShaderPass::CreateGS(const std::vector<char>& compiledBuffer)
{
	// シェーダー作成
	if (FAILED(D3D.GetDev()->CreateGeometryShader(&compiledBuffer[0], compiledBuffer.size(), nullptr, &m_GS)))
	{
		assert(0 && "ジオメトリシェーダー作成失敗");
		return false;
	}
	return true;
}

bool KdShaderPass::CreatePS(const std::vector<char>& compiledBuffer)
{
	// シェーダー作成
	if (FAILED(D3D.GetDev()->CreatePixelShader(&compiledBuffer[0], compiledBuffer.size(), nullptr, &m_PS)))
	{
		assert(0 && "ピクセルシェーダー作成失敗");
		return false;
	}
	return true;
}

void KdShader::LoadShaderAndSetPass(const std::string& folderPath, const std::string& hlslFilename,
	uint32_t passIndex, const std::string& passName,
	const std::string& vsProcName, const std::string& psProcName, const std::string& gsProcName)
{
	auto pass = std::make_shared<KdShaderPass>();
	pass->m_name = passName;

	std::vector<char> compiledBuffer;
	std::string errorMsg;

	// 頂点シェーダ
	if (vsProcName.empty() == false)
	{
		if (pass->LoadShader(folderPath, hlslFilename, vsProcName.c_str(), "vs_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "シェーダーコンパイルエラー", MB_ICONERROR | MB_OK);
			return;
		}

		// 頂点シェーダー作成
		if (pass->CreateVS(compiledBuffer) == false)return;
		// 頂点入力レイアウト作成
		if (pass->CreateInputLayout(compiledBuffer) == false)return;
	}

	// ピクセルシェーダ
	if (psProcName.empty() == false)
	{
		if (pass->LoadShader(folderPath, hlslFilename, psProcName.c_str(), "ps_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "シェーダーコンパイルエラー", MB_ICONERROR | MB_OK);
			return;
		}

		// ピクセルシェーダー作成
		if (pass->CreatePS(compiledBuffer) == false)return;
	}

	// ジオメトリシェーダ
	if (gsProcName.empty() == false)
	{
		if (pass->LoadShader(folderPath, hlslFilename, gsProcName.c_str(), "gs_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "シェーダーコンパイルエラー", MB_ICONERROR | MB_OK);
			return;
		}

		// ジオメトリシェーダー作成
		if (pass->CreateGS(compiledBuffer) == false)return;
	}

	// ロード情報を記憶しておく
	pass->m_loadInfo.folderPath = folderPath;
	pass->m_loadInfo.hlslFilename = hlslFilename;
	pass->m_loadInfo.passIndex = passIndex;
	pass->m_loadInfo.passName = passName;
	pass->m_loadInfo.vsProcName = vsProcName;
	pass->m_loadInfo.psProcName = psProcName;
	pass->m_loadInfo.gsProcName = gsProcName;

	// パスを追加
	SetPass(passIndex, pass);


}

void KdShader::ReloadShader()
{
	for (auto&& pass : m_passes)
	{
		LoadShaderAndSetPass(
			pass->m_loadInfo.folderPath,
			pass->m_loadInfo.hlslFilename,
			pass->m_loadInfo.passIndex,
			pass->m_loadInfo.passName,
			pass->m_loadInfo.vsProcName,
			pass->m_loadInfo.psProcName,
			pass->m_loadInfo.gsProcName
		);
	}

}
