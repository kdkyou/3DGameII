
#include "KdModelRendererComponent.h"
#include "KdAnimationComponent.h"

#include "KdFramework.h"
#include "Editor/KdEditorData.h"

SetClassAssembly(KdAnimationComponent, "Component");

KdAnimationComponent::KdAnimationComponent()
{
	
}

void KdAnimationComponent::Update()
{
	// コンポーネント自体が非アクティブ
	if(!m_enable){return;}

	// 管理しているモデル描画コンポーネント
	auto modelComp = m_modelComponent.lock();
	if (modelComp == nullptr) 
	{
		// アニメーションさせるモデルコンポーネントがいないのであれば検索
		SetModelComponent(GetGameObject()->GetComponent<KdModelRendererComponent>());
		return;
	}

	// アニメーションデータが有るか
	if(m_calculatedTransform == nullptr){return;}
	if(m_runtimeDatas.size() == 0) { return; }

	// 描画する予定のモデルデータ
	const auto modelData = modelComp->GetSourceModel();
	if(modelData == nullptr){return;}

	// 実行中のすべてのアニメーションの更新
	m_interpolatedResults.clear();
	bool isUse = false; // 有効なアニメーションが一つでも有るか
	for(auto&& runtime : m_runtimeDatas)
	{
		runtime.UpdateAnimation(modelComp, m_interpolatedResults);
		isUse |= runtime.isEnable;
	}
	if(isUse == false){return;}

	// 計算結果格納場所の再確保(アニメーションが適用されないところはデフォルトの位置)
	m_calculatedTransform = std::make_shared< std::vector<KdTransform> >();
	if (m_calculatedTransform)
	{
		modelData->CreateNodeTransforms(*m_calculatedTransform);
	}
	
	// 結果を合成していく
	for(auto&& result : m_interpolatedResults)
	{
		if( result.nodeIdx < 0 ){continue;}
		if( (*m_calculatedTransform).size() <= result.nodeIdx){continue;}

		auto& trans = (*m_calculatedTransform)[result.nodeIdx];

		// 拡縮 = 増減分(1からの変化量)を元々に加算
		auto vAddScale = (result.vScale - KdVector3( 1, 1, 1)) * result.weight;
		trans.SetLocalScale(trans.GetLocalScale() + vAddScale);

		// 回転 = 回転具合を元々と球面補完
		auto quat = KdQuaternion::Slerp(trans.GetLocalRotation() , result.qRotation, result.weight);
		trans.SetLocalRotation(quat);

		// 移動 = 移動具合を元々と線形補完
		auto vAddPosition = KdVector3::Lerp(trans.GetLocalPosition() , result.vPosition, result.weight);
		trans.SetLocalPosition( vAddPosition );
	}

	// 管理しているモデルに計算結果を伝える
	modelComp->SetAnimatedTransform(m_calculatedTransform);
}

// モデルデータからアニメーション情報を抜き出して独自形式に保存
void KdAnimationComponent::ConvertModelToAnimation()
{
	// 同一階層のモデル描画コンポーネントを探してとりあえず設定
	SetModelComponent(GetGameObject()->GetComponent<KdModelRendererComponent>());

	// 読み込んだモデルデータの全Animationデータを保存
	std::string filepath;
	if (KdEditorData::GetInstance().OpenFileDialog(filepath))
	{
		auto model = KdResourceManager::GetInstance().LoadAsset<KdModel>(filepath);

		// フォルダ名のみ抜き出し
		std::filesystem::path fs(filepath);
		std::string directoryPath = fs.parent_path().string();

		// 独自のアニメーションデータとして保存
		for(auto anim : model->GetAnimations())
		{
			anim.Save(directoryPath);
		}
	}

}

// アニメーションファイルを指定して読み込む
void KdAnimationComponent::LoadFromAnimationDataWithDialog()
{
	// アニメーション毎に必要なデータの準備
	std::string selectFilePath;
	if (KdEditorData::GetInstance().OpenFileDialog(selectFilePath, u8"アニメーションデータを開く", "*.kdanim"))
	{
		LoadFromAnimationData(selectFilePath);
	}
}

KdAnimationComponent::RuntimeAnimationData& KdAnimationComponent::LoadFromAnimationData(const std::string& path)
{
	// AnimationDataを読み込む
	auto anim = KdResourceManager::GetInstance().LoadAsset<KdAnimationData>(path);

	// AnimationDataと関連付けた実行時データを作成
	RuntimeAnimationData runtime;
	runtime.AniamtionData = anim;
	runtime.AnimationDataPath = path;
	runtime.AnimationName = anim->Name; // 管理名は最初はファイル名
	m_runtimeDatas.push_back(runtime);

	// すでにモデルがあればアニメーションデータと突き合わせておく
	auto modelComp = m_modelComponent.lock();
	if(modelComp && modelComp->GetModel() != nullptr)
	{
		runtime.CreateApplyIndexList(modelComp->GetModel()->GetAllNodes());
	}

	// さっき入った要素を返す
	return *(m_runtimeDatas.end() - 1);
}

//KdAnimationComponent::RuntimeAnimationData& KdAnimationComponent::LoadFromAnimationData(const std::string& path, const float speed, const float startTime)
//{
//	// AnimationDataを読み込む
//	auto anim = KdResourceManager::GetInstance().LoadAsset<KdAnimationData>(path);
//
//	// AnimationDataと関連付けた実行時データを作成
//	RuntimeAnimationData runtime;
//	runtime.AniamtionData = anim;
//	runtime.AnimationDataPath = path;
//	runtime.AnimationName = anim->Name; // 管理名は最初はファイル名
//	runtime.speed = speed;
//	runtime.StartTime = startTime;
//	m_runtimeDatas.push_back(runtime);
//
//	// すでにモデルがあればアニメーションデータと突き合わせておく
//	auto modelComp = m_modelComponent.lock();
//	if (modelComp && modelComp->GetModel() != nullptr)
//	{
//		runtime.CreateApplyIndexList(modelComp->GetModel()->GetAllNodes());
//	}
//
//	// さっき入った要素を返す
//	return *(m_runtimeDatas.end() - 1);
//}


// アニメーションを適用させるモデルコンポーネントの指定
void KdAnimationComponent::SetModelComponent(std::shared_ptr<KdModelRendererComponent> modelComp)
{
	if(modelComp == nullptr){return;}
	if(modelComp->GetModel() == nullptr){return;}

	m_modelComponent = modelComp;
	auto model = modelComp->GetSourceModel();
	if(model == nullptr){return;}

	// 計算結果を格納する領域確保
	m_calculatedTransform = std::make_shared< std::vector<KdTransform> >();
	if(m_calculatedTransform)
	{
		model->CreateNodeTransforms(*m_calculatedTransform);
	}

	// 適用インデックスリスト作成
	for(auto&& anim : m_runtimeDatas)
	{
		anim.CreateApplyIndexList(model->GetAllNodes());
	}
}


// アニメーション名を検索して実行
void KdAnimationComponent::Play(const std::string& name, bool isLoop, float crossFadeTime)
{
	auto modelComp = m_modelComponent.lock();
	if(modelComp == nullptr){return;}

	auto model = modelComp->GetSourceModel();
	if(model == nullptr){return;}

	for(auto&& anim : m_runtimeDatas)
	{
		if(anim.AnimationName == name)
		{
			// クロスフェードの時間が設定されていた(されていても再生中のアニメーションがなければ即時移動)
			if (crossFadeTime > 0 && IsPlaying())
			{
				anim.ease = std::make_shared<KdEase>();
				anim.ease->Start(KdEase::Type::Straight, crossFadeTime, false);

				// 現在再生中のアニメーションにフェードアウトを差し込んでいく
				for (auto&& nowProg : m_runtimeDatas)
				{
					if(nowProg.isPlaying)
					{
						nowProg.ease = std::make_shared<KdEase>();
						nowProg.ease->Start(KdEase::Type::Straight, crossFadeTime, true);
					}
				}
			}
			else
			{
				anim.ease = nullptr;
				Stop(); // 他のアニメーションは停止して即座に移動
			}

			// アニメーション再生設定
			anim.isEnable	= true;
			anim.isPlaying	= true;
			anim.isLoop		= isLoop;
			anim.weight		= 1.0f;

			//何処から再生するか
			anim.ProgressTime = anim.StartTime;
		}
	}
}

void KdAnimationComponent::Play(const std::string& name, float animationSpeed, bool isLoop, float crossFadeTime)
{
	auto modelComp = m_modelComponent.lock();
	if (modelComp == nullptr) { return; }

	auto model = modelComp->GetSourceModel();
	if (model == nullptr) { return; }

	for (auto&& anim : m_runtimeDatas)
	{
		if (anim.AnimationName == name)
		{
			// クロスフェードの時間が設定されていた(されていても再生中のアニメーションがなければ即時移動)
			if (crossFadeTime > 0 && IsPlaying())
			{
				anim.ease = std::make_shared<KdEase>();
				anim.ease->Start(KdEase::Type::Straight, crossFadeTime, false);

				// 現在再生中のアニメーションにフェードアウトを差し込んでいく
				for (auto&& nowProg : m_runtimeDatas)
				{
					if (nowProg.isPlaying)
					{
						nowProg.ease = std::make_shared<KdEase>();
						nowProg.ease->Start(KdEase::Type::Straight, crossFadeTime, true);
					}
				}
			}
			else
			{
				anim.ease = nullptr;
				Stop(); // 他のアニメーションは停止して即座に移動
			}

			// アニメーション再生設定
			anim.isEnable = true;
			anim.isPlaying = true;
			anim.isLoop = isLoop;
			anim.weight = 1.0f;
			anim.speed = animationSpeed;
			anim.ProgressTime = anim.StartTime;
		}
	}
}

void KdAnimationComponent::Stop(const std::string& stopAnimationName)
{
	bool isAll = false;
	if(stopAnimationName == ""){isAll = true;}

	for (auto&& runtime : m_runtimeDatas)
	{
		if(isAll || runtime.AnimationName == stopAnimationName)
		{
			runtime.isPlaying = false;
			runtime.isEnable = false;
		}
	}
}

void KdAnimationComponent::Suspend(const std::string& suspendAnimationName)
{
	bool isAll = false;
	if (suspendAnimationName == "") { isAll = true; }

	for (auto&& runtime : m_runtimeDatas)
	{
		if (isAll || runtime.AnimationName == suspendAnimationName)
		{
			runtime.isPlaying = false;
		}
	}
}

bool KdAnimationComponent::IsPlaying(const std::string& animationName) const
{
	// アニメーション名指定検索
	if(animationName != "")
	{
		for (auto&& runtime : m_runtimeDatas)
		{
			if (runtime.AnimationName == animationName && runtime.isPlaying) { return true; }
		}
		return false;
	}

	// 全検索
	for(auto&& runtime : m_runtimeDatas)
	{
		if(runtime.isPlaying){return true;}
	}
	return false;
}

void KdAnimationComponent::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	auto arrJson = jsonObj["AnimationDatas"];
	for(UINT idx = 0; idx < (UINT)arrJson.size(); idx++)
	{
		// アニメーションデータはファイル名を覚えているだけなので、再度作成
		std::string path;
		KdJsonUtility::GetValue(arrJson.at(idx), "AnimationDataPath", &path);
		auto runtime = LoadFromAnimationData(path);

		// 実行時データの復元
		KdJsonUtility::GetValue(arrJson.at(idx), "AnimationName", &runtime.AnimationName);

		//自身で追加したもの
		//アニメーション速度
		KdJsonUtility::GetValue(arrJson.at(idx), "AnimationSpeed", &runtime.speed);
		//アニメーション初期開始時間
		KdJsonUtility::GetValue(arrJson.at(idx), "AnimationStartTime", &runtime.StartTime);
	}
}

void KdAnimationComponent::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	auto arr = nlohmann::json::array();
	for(auto&& anim : m_runtimeDatas)
	{
		nlohmann::json obj;
		obj["AnimationDataPath"] = anim.AnimationDataPath;
		obj["AnimationName"] = anim.AnimationName;

		//自身で追加したもの
		obj["AnimationSpeed"] = anim.speed;
		obj["AnimationStartTime"] = anim.StartTime;
		
		arr.push_back(obj);
	}
	outJson["AnimationDatas"] = arr;
}

void KdAnimationComponent::Editor_ImGui()
{	
	
	KdComponent::Editor_ImGui();

	// 管理しているアニメーション一覧
	if (ImGui::CollapsingHeader(u8"アニメーション一覧", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for(auto&& anim : m_runtimeDatas)
		{
			if(anim.AniamtionData == nullptr){continue;}
			ImGui::PushID(&anim);
			if (ImGui::CollapsingHeader(anim.AnimationName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::InputText( u8"管理名", &anim.AnimationName);
				ImGui::LabelText(std::to_string(anim.AniamtionData->LengthInSecond).c_str(), u8"総再生時間");
				
				//アニメーション情報をキャラクターに適応するか
				if(anim.isEnable)
				{
					ImGui::LabelText(std::to_string(anim.weight).c_str(), u8"適用中");
				}

				if (anim.isPlaying)
				{
					ImGui::LabelText( std::to_string(anim.ProgressTime).c_str(), u8"再生中");
				}

				if (ImGui::Button(u8"Play"))
				{
					Play(anim.AnimationName, false, 3.0f);
				}

				//アニメーション速度調整
				ImGui::DragFloat(u8"進行速度", &anim.speed, 0.01f, 0.0f, 100.0f);

				//アニメーション初期時間
				ImGui::DragFloat(u8"初期時間", &anim.StartTime, 0.01f, 0.0f, 1.0f);


			}
			ImGui::PopID();
		}
	}

	// ファイルの読み込み
	if (ImGui::CollapsingHeader(u8"ファイル読み込み", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::Button(u8"モデルデータからアニメーションデータへコンバート"))
		{
			ConvertModelToAnimation();
		}

		if(ImGui::Button(u8"アニメーションデータから読み込み"))
		{
			LoadFromAnimationDataWithDialog();
		}

		ImGui::Unindent();
	}
}


// 有効になっているアニメーションを更新して補完結果を計算する
bool KdAnimationComponent::RuntimeAnimationData::UpdateAnimation(std::shared_ptr<KdModelRendererComponent> modelComp, std::vector<InterpolatedResult>& results)
{
	
	if (isEnable == false) { return false; }

	auto& modelData = modelComp->GetSourceModel();

	bool isEnd = false;

	auto& time = KdTime::GetInstance();

	// 時間の更新とループ
	if (isPlaying)
	{ 
		ProgressTime += time.GetDeltaTime() * speed;
		if (ProgressTime > AniamtionData->LengthInSecond)
		{
			isEnd = true;
			if (isLoop)
			{
				ProgressTime = ProgressTime - AniamtionData->LengthInSecond;
			}
			else
			{
				ProgressTime = AniamtionData->LengthInSecond;
				isPlaying = false;
				isEnable = false;
			}
		}
	}

	// フェードIN OUT
	if(ease)
	{
		weight = ease->NextValue();
		if(ease->IsEnd())
		{
			ease = nullptr;
			// フェードが終わっていてかつ、weightが0だったらアニメーションが終わったと判断
			if(weight <= 0.01f)
			{
				isPlaying = false;
				isEnable = false;
			}
		}
	}

	// 進行具合を0-1で
	double progressPer = ProgressTime / AniamtionData->LengthInSecond;

	// 動作確認用
	int useFrame = (int)(progressPer * 60);

	// ノード毎に補完
	for (auto&& perNode : AniamtionData->PerNodes)
	{
		const std::string& nodeName = perNode.Name; // アニメーションさせるノード名

		// 管理するモデルコンポーネントに該当するノードがあるか
		auto node = modelData->GetNode(nodeName);
		//if (node == nullptr) { continue; }

		// 各要素の補間
		auto interpolation = AniamtionData->Interpolation(perNode, ProgressTime);

		// 結果をスタック
		InterpolatedResult result;
		result.weight = weight;
		result.vPosition = interpolation.GetLocalPosition();
		result.qRotation = interpolation.GetLocalRotation();
		result.vScale	 = interpolation.GetLocalScale();

		if (node != nullptr)
		{
			result.nodeIdx = node->NodeIndex;

#ifdef __DEV_MODE
			std::string modelNodeName = node->NodeName;
#endif
		}

		results.push_back(result);
	}

	return isEnd;
}

bool KdAnimationComponent::RuntimeAnimationData::CreateApplyIndexList(const std::vector<KdModel::Node>& modelNodes)
{
	if(AniamtionData == nullptr){return false;}

	ApplyNodeIndex.clear();
	ApplyNodeIndex.resize(AniamtionData->PerNodes.size());

	for(auto&& modelNode : modelNodes)
	{
		bool hit = false;

		// ランタイムが覚える方式
		for(UINT idx = 0; idx < AniamtionData->PerNodes.size(); idx++)
		{
			if (modelNode.NodeName == AniamtionData->PerNodes[idx].Name)
			{
				ApplyNodeIndex[idx] = modelNode.NodeIndex;
				hit = true;
			}
		}

		// アニメーションデータに埋め込む方式→廃止予定
		for(auto&& animNode : AniamtionData->PerNodes)
		{
			if(modelNode.NodeName == animNode.Name)
			{
				animNode.NodeIndex = modelNode.NodeIndex;
				hit = true;
			}
		}

		if(hit == false)
		{
			// return false;
		}
	}

	return true;
}







