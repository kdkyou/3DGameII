#pragma once

// ゲームに使用するAsset(読み込まれる前の素材データ)
// の管理を行うクラス

class KdAssetManager 
{
public :

	void Initialize();
	void Editor_ImGui();
	// アセットフォルダ内を検索して、対応しているAssetファイルに
	// メタファイル(Guidの保存場所 )を作っていく
	void CreateMetaFileForAllAssets();
	// メタファイルをすべて削除する関数 ※危険
	void DeleteAllMetaFiles();


private:

	// 対応しているアセットの拡張子
	std::vector<std::string> m_supportedExtentions;
	// 対応しているファイル拡張子か調べる
	bool IsSupportedAsset(const std::filesystem::path& srcFile);

	// アセットが入っているフォルダ
	std::string m_assetsFilePath = "./Assets/";
	// 作成するメタファイルの拡張子
	std::string m_metaFileExtention = ".astsmeta";


 // シングルトン
 // インスタンスを一つしか作らないパターン
public:

	static KdAssetManager& GetInstance()
	{
		static KdAssetManager instance; // 一度しか領域確保されない
		return instance;
	}


private:

	KdAssetManager(){} // 外側からインスタンス化禁止

};