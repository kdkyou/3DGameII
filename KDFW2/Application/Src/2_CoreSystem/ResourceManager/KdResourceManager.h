#pragma once

#include "KdObject.h"

// アセット管理クラス
class KdResourceManager
{
public:

	// アセットをロードする(型指定バージョン)
	template<class AssetType>
	std::shared_ptr<AssetType> LoadAsset(const std::string& assetPath)
	{
		return std::dynamic_pointer_cast<AssetType>( LoadAsset(assetPath) );
	}

	// アセットをロードする
	std::shared_ptr<KdObject> LoadAsset(const std::string& assetPath);

	// アセットをロードする関数を登録する
	void RegisterAssetLoadFunction(const std::string& typeName, std::function<std::shared_ptr<KdObject>(const std::string&)> func)
	{
		m_loadFunctions[typeName] = func;
	}

	// 読み込み済みのアセット管理をクリアする
	void ReleaseAllLoadedAssets()
	{
		m_loadedAssetMap.clear();
	}

private:

	// 読み込み済みアセット管理マップ
	std::unordered_map<std::string, std::shared_ptr<KdObject>> m_loadedAssetMap;

	// アセットロード関数登録マップ
	// 例)"Texture" -> テクスチャをロードする関数を登録しておく
	// 例)"Model" -> モデルをロードする関数を登録しておく
	std::unordered_map<
		std::string,
		std::function<std::shared_ptr<KdObject>(const std::string&)>
	> m_loadFunctions;

//-------------------------------
// シングルトン
//-------------------------------
private:

	KdResourceManager()
	{
	}

public:
	static KdResourceManager& GetInstance() {
		static KdResourceManager instance;
		return instance;
	}

};