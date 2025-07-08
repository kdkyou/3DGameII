#include "KdResourceManager.h"

std::shared_ptr<KdObject> KdResourceManager::LoadAsset(const std::string& assetPath)
{
	// 既に読み込み済みなら、それを返す。
	{
		auto it = m_loadedAssetMap.find(assetPath);
		if (it != m_loadedAssetMap.end())
		{
			return (*it).second;
		}
	}

	// メタファイルがある場合は、情報取得
	nlohmann::json metaData;
	std::ifstream ifs(assetPath + ".kdfwmeta");
	if (ifs)
	{
		// メタファイル読み込み
		ifs >> metaData;

		// ファイルタイプから、適切な読み込み関数を実行する
		std::string type = metaData["Type"].get<std::string>();

		// Typeから、読み込み処理を選択
		auto itFound = m_loadFunctions.find(type);
		if (itFound == m_loadFunctions.end())return nullptr;

		// 読み込み実行
		auto loadedAsset = itFound->second(assetPath);
		if (loadedAsset != nullptr)
		{
			// 読み込み済みへ登録
			m_loadedAssetMap[assetPath] = loadedAsset;

			return loadedAsset;
		}
	}
	else
	{
		// メタファイル情報がないなら、総あたりで読み込み関数を実行
		for (auto&& func : m_loadFunctions)
		{
			// 読み込み実行
			auto loadedAsset = func.second(assetPath);
			if (loadedAsset != nullptr)
			{
				// 読み込み済みへ登録
				m_loadedAssetMap[assetPath] = loadedAsset;

				return loadedAsset;
			}
		}
	}



	return nullptr;
}
