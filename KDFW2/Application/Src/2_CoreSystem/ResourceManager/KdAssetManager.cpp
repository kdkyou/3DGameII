#include"KdAssetManager.h"

#include"Editor/KdEditorData.h"

void KdAssetManager::Initialize()
{
	// 対応する拡張子の登録=>最終的には外部ファイル
	m_supportedExtentions.clear();
	m_supportedExtentions.push_back(".png");
	// 追加

	// データの行進
	CreateMetaFileForAllAssets();
}

void KdAssetManager::Editor_ImGui()
{

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
				ofs << "Metaファイルですよ";

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
