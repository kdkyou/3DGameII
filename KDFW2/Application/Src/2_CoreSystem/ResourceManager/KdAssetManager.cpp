#include"KdAssetManager.h"

#include"Editor/KdEditorData.h"

void KdAssetManager::CopyAssetsKeepFolders(const std::string& outPath) const
{
	// コピー元とコピー先が同じ
	if (outPath == m_assetsFilePath) { return; }

	// 吐き出し先のフォルダパスを作成
	std::filesystem::path outDir(outPath);
	// 既に出力先があるかどうか
	if (std::filesystem::exists(outPath)  == true)
	{
		// 一旦すべて削除
		std::filesystem::remove_all(outPath);
	}

	// 出力先を作成
	std::filesystem::create_directories(outDir);

	// コピー元のディレクトリをクロール
	for (auto& entry : std::filesystem::recursive_directory_iterator(m_assetsFilePath))
	{
		// メタファイルかどうか
		if(entry.path().extension().string() != m_metaFileExtention){continue;}

		// メタファイルのパス作成
		auto metaFilePath = entry.path();
		// メタファイルのパスからAssetのパスを作成
		auto assetFilePath = entry.path();
		assetFilePath.replace_extension("");

		// アセットデータが存在しない
		if (std::filesystem::exists(assetFilePath) == false) { continue; }

		// 出力先のパスを作成
		auto outDir = outPath / metaFilePath;
		outDir.remove_filename();	// ディレクトリ情報だけにする
		// 出力先のディレクトリの作成
		std::filesystem::create_directories(outDir);

		// メタファイルのコピー
		std::filesystem::copy_file(metaFilePath,outPath/metaFilePath);
		// Assetファイルのコピー
		std::filesystem::copy_file(assetFilePath, outPath / assetFilePath);


		// Log出力
		KdEditorData::GetInstance().m_logWindow.AddLog(u8"Assetファイルとメタファイルのコピー：%s", assetFilePath.filename().string().c_str());
	}

	//ログ出力
	KdEditorData::GetInstance().m_logWindow.AddLog(u8"Assetファイルコピー");



}

void KdAssetManager::CreateRuntimeData()
{
	m_assets.clear();

	m_selAsset = nullptr; // 選択しているものがなくなるのでリセット

	// 指定フォルダ以下のMetaファイルを探す
	for (auto& entry : std::filesystem::recursive_directory_iterator(m_assetsFilePath))
	{
		// 通常ファイル以外(フォルダとか隠しファイル)
		if (entry.is_regular_file() == false) { continue; }

		// Metaファイルではなかった
		if (entry.path().extension().string() != m_metaFileExtention){continue;}

		// Metaファイルの読み込み
		std::ifstream ifs(entry.path().string());
		if (ifs.fail() == true) { continue; }

		// Jsonデータの解析
		nlohmann::json json;
		ifs >> json;

		// ランタイムデータの作成
		AssetPropety p = {};
		
		// Guid
		KdJsonUtility::GetValue(json, "GUID", &p.guid);

		//　現在のAssetのパス
		auto resPath = entry.path();
		resPath.replace_extension(""); // メタファイルの拡張子を消す
		p.FilePath = resPath.string();

		// ファイル名だけ
		p.FileName = resPath.filename().string();
		// Guidをキーにしてリストに登録
		m_assets[p.guid] = p;

	}

}

const std::string& KdAssetManager::GetFilePathWithGuid(const std::string guid)const
{
	// TODO: return ステートメントをここに挿入します
	if (m_assets.find(guid) != m_assets.end()) 
	{ 
		return m_assets.at(guid).FilePath;
	}

	static std::string nofile = "";
	return nofile;

}

const KdAssetManager::AssetPropety* KdAssetManager::GetAssetPropertyWithGuid(const std::string& guid)const
{
	if (m_assets.find(guid) != m_assets.end())
	{
		return &m_assets.at(guid);
	}

	return nullptr;
}

void KdAssetManager::Initialize()
{
	// 対応する拡張子の登録=>最終的には外部ファイル
	m_supportedExtentions.clear();
	m_supportedExtentions.push_back(".png");
	m_supportedExtentions.push_back(".cso");
	//m_supportedExtentions.push_back(".gltf");
	//m_supportedExtentions.push_back(".kdanim");
	//m_supportedExtentions.push_back(".kdprefab");
	//m_supportedExtentions.push_back(".scene");



	// データの更新
	CreateMetaFileForAllAssets();

	// ゲーム中に使用するデータの更新
	CreateRuntimeData();
}

void KdAssetManager::Editor_ImGui()
{
	if (ImGui::TreeNode(u8"アセット一覧"))
	{
		// アセット一覧
		for (auto& asset : m_assets)
		{
			// 現在選択中か
			bool sel = false;
			if (m_selAsset != NULL && m_selAsset->guid == asset.first)
			{
				sel = true;
			}
			if (ImGui::Selectable(asset.second.FilePath.c_str()) == true)
			{
				// 項目を選択した
				m_selAsset = &asset.second;
			}
		}
		ImGui::TreePop();
	}
}

void KdAssetManager::CreateMetaFileForAllAssets()
{
	// 指定フォルダ(Assets)以下をクロール
	for (const std::filesystem::directory_entry& entry : 
		// フォルダもファイルも全て含めたリスト
	       std::filesystem::recursive_directory_iterator(m_assetsFilePath))
	{

		// 通常のファイルか かつ 管理対象の拡張子か
		bool flg = entry.is_regular_file();
		if (entry.is_regular_file() && IsSupportedAsset(entry.path()))
		{
			// このファイルのパス
			std::string filePath = entry.path().string();

			// メタファイルの名前
			// ファイル名.拡張子.メタファイルの拡張子
			// flare.png.astsmeta
			std::filesystem::path metafilePath = entry.path();
			metafilePath.replace_filename(entry.path().filename().string() + m_metaFileExtention);

			// このメタファイルが存在しているか
			if (std::filesystem::exists(metafilePath) == false)
			{
				// メタファイルの新規作成
				std::ofstream ofs(metafilePath);

				// メタファイルの情報を作成
				ofs << CreateMetaData(entry.path());

				// Logの出力
				KdEditorData::GetInstance().m_logWindow.AddLog(u8"Metaファイル作成：%s", filePath.c_str());
			}
		}
	}
}

void KdAssetManager::DeleteAllMetaFiles()
{
	// Assetsフォルダ以下をクロール
	for (auto& entry : std::filesystem::
		recursive_directory_iterator(m_assetsFilePath))
	{
		// メタファイルか
		if (entry.path().extension().string() != m_metaFileExtention){continue;}

		// 削除
		if (std::filesystem::remove(entry.path()))
		{
			std::string filePath = entry.path().string();
			KdEditorData::GetInstance().m_logWindow.AddLog(u8"Metaファイルを削除：%s", filePath.c_str());
		}

	}
}

// 1Assetに対するMetaデータの作成
nlohmann::json KdAssetManager::CreateMetaData(const std::filesystem::path& srcFile)
{
	nlohmann::json json;
	// 新しいGuid
	KdGuid guid;
	json["GUID"] = guid.ToString();

	// データタイプ
	// 付随データ
	// 等が増えていくはず

	return json;
}

bool KdAssetManager::IsSupportedAsset(const std::filesystem::path& srcFile)
{
	// 拡張子のみを文字列化
	std::string fileExt = srcFile.extension().string();
	for (auto& ext : m_supportedExtentions)
	{
		if (fileExt == ext){ return true; } // サポートファイル
	}

	return  false; // サポートファイルではない

}
