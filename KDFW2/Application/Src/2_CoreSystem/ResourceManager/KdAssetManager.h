#pragma once

// ゲームに使用するAsset(読み込まれる前の素材データ)
// の管理を行うクラス

class KdAssetManager 
{
public :
	// ゲーム実行中に称するAssetの情報
	struct AssetPropety {
		std::string guid = "";
		std::string FilePath = ""; // 実行直後に調べる
		std::string FileName = ""; // ファイル名のみ
	};

	// 実行時に必要なデータを作成する
	void CreateRuntimeData();

	// 現在選択中のAsset
	const AssetPropety* GetSelectedAsset() const { return m_selAsset; }
	// GuidからAssetのファイルパスを送る関数
	const std::string& GetFilePathWithGuid(const std::string guid) const;
	// GuidからAssetPropertyの取得
	const AssetPropety* GetAssetPropertyWithGuid(const std::string& guid)const;

private:

	// 実行時に必要なデータリスト
	// キー(std::string)：ファイルのGuid
	std::map<std::string, AssetPropety> m_assets;

	//エディター操作
	AssetPropety* m_selAsset = nullptr;

public :

	void Initialize();
	void Editor_ImGui();
	// アセットフォルダ内を検索して、対応しているAssetファイルに
	// メタファイル(Guidの保存場所 )を作っていく
	void CreateMetaFileForAllAssets();
	// メタファイルをすべて削除する関数 ※危険
	void DeleteAllMetaFiles();



private:

	// Metaファイルの内容を作成する
	nlohmann::json CreateMetaData(const std::filesystem::path& srcFile);

	// 対応しているアセットの拡張子
	std::vector<std::string> m_supportedExtentions;
	// 対応しているファイル拡張子か調べる
	bool IsSupportedAsset(const std::filesystem::path& srcFile);

	// アセットが入っているフォルダ
	std::string m_assetsFilePath = "./Assets/";
	// 作成するメタファイルの拡張子
	std::string m_metaFileExtention = ".kdfwmeta";


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