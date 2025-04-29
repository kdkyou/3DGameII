#pragma once

#define __DEV_MODE

//=========================================================
// アニメーションの管理コンポーネント
//=========================================================
#include "KdComponent.h"
class KdModel;

class KdAnimationComponent : public KdComponent
{
public:

	// それぞれのノードを計算した結果
	struct InterpolatedResult
	{
		int nodeIdx = -1;		// どのノードに対する結果か
		float weight = 1.0f;	// このアニメーションの利用割合

		enum CalcFlag { none = 0, pos = 0x01, rot = 0x02, scale = 0x04 };
		CalcFlag calcFlag = none;	// どの要素に計算が行われたか

		KdVector3 vPosition;
		KdVector3 vScale;
		KdQuaternion qRotation;

#ifdef __DEV_MODE
		std::string modelNodeName = "";
#endif
	};

	// アニメーションデータに再生時に必要なデータを付随させたもの
	struct RuntimeAnimationData
	{
		std::string		AnimationDataPath = "";			// 保存用に読み込みに使ったファイルパス
		std::string		AnimationName = "";				// ゲーム内で管理用のアニメーション名

		// 再生するアニメーションデータ
		std::shared_ptr<KdAnimationData> AniamtionData = nullptr;

		std::vector<UINT> ApplyNodeIndex;				// 各補完データがどのノードに適用されるか
#ifdef __DEV_MODE
		std::vector < std::string > ApplyNodeName;
#endif

		double ProgressTime = 0.0;						// 現在の進行時間
		float StartTime = 0.0;					// 初期の進行時間

		bool isEnable = false;							// このアニメーションを合成に使うかどうか
		bool isPlaying = false;							// このアニメーションを更新するかどうか
		bool isLoop = false;							// ループするかどうか

		float speed = 1.0f;								// 再生スピード(1.0でアニメーションを作った通り)
		float weight = 1.0f;							// このアニメーションの利用割合(1.0で100%適用)

		std::shared_ptr<KdEase> ease = nullptr;			// weightの変化のさせ方

		// アニメーション一つを更新する
		// 戻り値：そのアニメーションが終わった瞬間
		bool UpdateAnimation(std::shared_ptr<KdModelRendererComponent> modelComp, std::vector<InterpolatedResult>& out);
		bool CreateApplyIndexList(const std::vector<KdModel::Node>& modelNodes);


	};

	KdAnimationComponent();

	virtual void Update() override;
	virtual void Draw(){}

	// アニメーションデータの読み込み
	// 現状モデルデータから直で読み込むしか方法がない
	void ConvertModelToAnimation();

	// 独自アニメーションデータから読み込み
	void LoadFromAnimationDataWithDialog();					// ファイル選択ダイアログ表示
	RuntimeAnimationData& LoadFromAnimationData(const std::string& path);
//	RuntimeAnimationData& LoadFromAnimationData(const std::string& path,const float speed,const float startTime);

	// アニメーションを関連付けるモデル
	void SetModelComponent( std::shared_ptr<KdModelRendererComponent> modelComp );
	
	// アニメーションの再生
	void Play(const std::string& name, bool isLoop = false, float crossFadeTime = 0.1f);
	void Play(const std::string& name, float animationSpeed, bool isLoop = false, float crossFadeTime = 0.1f);

	// アニメーション再生の停止(更新も適用も終わる) 指定がなかったら全部止める
	void Stop(const std::string& stopAnimationName = "");

	// アニメーションの中断(再生が止まるだけで適用はそのまま)
	void Suspend(const std::string& suspendAnimationName = "");

	bool IsPlaying(const std::string& animationName = "") const; // 現在再生中のアニメーションが有るかどうか

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const nlohmann::json& jsonObj);
	virtual void Serialize(nlohmann::json& outJson) const;


	// ImGui
	virtual void Editor_ImGui() override;

private:

	// アニメーションさせるモデルコンポーネント
	std::weak_ptr<KdModelRendererComponent> m_modelComponent;
	std::vector<RuntimeAnimationData> m_runtimeDatas;
	std::vector<InterpolatedResult> m_interpolatedResults; // すべてのアニメーションから報告される補完結果
	// 有効なすべてのアニメーションを合成した結果
	std::shared_ptr< std::vector<KdTransform> > m_calculatedTransform = nullptr;
};