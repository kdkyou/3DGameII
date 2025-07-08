#pragma once



//==========================================================
// 一つのアニメーションのデータ
//
//==========================================================
struct aiAnimation;
struct PerNode;

struct KdAnimationData : public KdObject
{
public:
	// Assimpから読み込んで来たデータをこちらのフォーマットに変換
	void Parse(aiAnimation* src, const std::vector<KdMeshScene::NodeData>& allNodes);

	// 保存
	bool Save(const std::string& directory)const;
	nlohmann::json Serialize() const;

	// 読み込み
	bool Load(const std::string& filepath);
	bool Deserialize(const nlohmann::json& jsonObj);

	// アニメーションデータのファイルパス
	std::string FilePath = "";

	// このアニメーション全体の情報
	std::string Name = "";
	double LengthInTick = 0.0f;		// Tick(多分フレームと同義)単位での長さ
	double TicksParSecond = 0.0f;	// 一秒間の進行Tick数(0の可能性あり)
	double LengthInSecond = 0.0f;	// アニメーションの継続時間（秒単位）

	// 1パーツ(ボーン)ごとの情報
	struct PerNode
	{
		std::string Name = "";		// 対象ノードの名前
		int NodeIndex = -1;			// 対応するノードのIndex→コンポーネントで管理

		// フレーム(姿勢)単位での移動・回転・拡縮のデータ
		struct PositionKey
		{
			double Time = 0;
			KdVector3 vPos;
			KdVector3 Interpolation(const PositionKey& last, double time) const;
		};
		struct RotationKey
		{
			double Time = 0;
			KdQuaternion qRot;

			// 呼び出したキーを先頭、引数を最後として二点間のキーの補完
			KdQuaternion Interpolation(const RotationKey& last, double time)const;
		};
		struct ScaleKey
		{
			double Time = 0;
			KdVector3 vScale;
			KdVector3 Interpolation(const ScaleKey& last, double time)const;
		};

		// Index指定でのキー取得
		const PositionKey&	GetPositionKey(int idx) const;
		const RotationKey&	GetRotationKey(int idx) const;
		const ScaleKey&		GetScaleKey(int idx) const;

		// フレーム指定での前後のキー取得
		std::tuple<const PositionKey&, const PositionKey&>	GetPositionKeyByTime(double frame) const;
		std::tuple<const RotationKey&, const RotationKey&>	GetRotationKeyByTime(double frame) const;
		std::tuple<const ScaleKey&, const ScaleKey&>		GetScaleKeyByTime(double frame) const;

		// フレーム指定での要素の補完
		KdVector3		InterpolationPosition(double time) const;
		KdQuaternion	InterpolationRotation(double time) const;
		KdVector3		InterpolationScale(double time) const;

		// Save Load
		nlohmann::json Serialize()const;
		bool Deserialize(const nlohmann::json& jsonObj);

		// 各データ
		std::vector<PositionKey> PositionKeys;
		std::vector<RotationKey> RotationKeys;
		std::vector<ScaleKey> ScaleKeys;
	};
	std::vector<PerNode> PerNodes;

	// 指定されたノードの補完を行う…前方宣言で解決できないのはなんでなんだぜ
	KdTransform Interpolation(const PerNode& node, double time);
};