
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
	// �R���|�[�l���g���̂���A�N�e�B�u
	if(!m_enable){return;}

	// �Ǘ����Ă��郂�f���`��R���|�[�l���g
	auto modelComp = m_modelComponent.lock();
	if (modelComp == nullptr) 
	{
		// �A�j���[�V���������郂�f���R���|�[�l���g�����Ȃ��̂ł���Ό���
		SetModelComponent(GetGameObject()->GetComponent<KdModelRendererComponent>());
		return;
	}

	// �A�j���[�V�����f�[�^���L�邩
	if(m_calculatedTransform == nullptr){return;}
	if(m_runtimeDatas.size() == 0) { return; }

	// �`�悷��\��̃��f���f�[�^
	const auto modelData = modelComp->GetSourceModel();
	if(modelData == nullptr){return;}

	// ���s���̂��ׂẴA�j���[�V�����̍X�V
	m_interpolatedResults.clear();
	bool isUse = false; // �L���ȃA�j���[�V��������ł��L�邩
	for(auto&& runtime : m_runtimeDatas)
	{
		runtime.UpdateAnimation(modelComp, m_interpolatedResults);
		isUse |= runtime.isEnable;
	}
	if(isUse == false){return;}

	// �v�Z���ʊi�[�ꏊ�̍Ċm��(�A�j���[�V�������K�p����Ȃ��Ƃ���̓f�t�H���g�̈ʒu)
	m_calculatedTransform = std::make_shared< std::vector<KdTransform> >();
	if (m_calculatedTransform)
	{
		modelData->CreateNodeTransforms(*m_calculatedTransform);
	}
	
	// ���ʂ��������Ă���
	for(auto&& result : m_interpolatedResults)
	{
		if( result.nodeIdx < 0 ){continue;}
		if( (*m_calculatedTransform).size() <= result.nodeIdx){continue;}

		auto& trans = (*m_calculatedTransform)[result.nodeIdx];

		// �g�k = ������(1����̕ω���)�����X�ɉ��Z
		auto vAddScale = (result.vScale - KdVector3( 1, 1, 1)) * result.weight;
		trans.SetLocalScale(trans.GetLocalScale() + vAddScale);

		// ��] = ��]������X�Ƌ��ʕ⊮
		auto quat = KdQuaternion::Slerp(trans.GetLocalRotation() , result.qRotation, result.weight);
		trans.SetLocalRotation(quat);

		// �ړ� = �ړ�������X�Ɛ��`�⊮
		auto vAddPosition = KdVector3::Lerp(trans.GetLocalPosition() , result.vPosition, result.weight);
		trans.SetLocalPosition( vAddPosition );
	}

	// �Ǘ����Ă��郂�f���Ɍv�Z���ʂ�`����
	modelComp->SetAnimatedTransform(m_calculatedTransform);
}

// ���f���f�[�^����A�j���[�V�������𔲂��o���ēƎ��`���ɕۑ�
void KdAnimationComponent::ConvertModelToAnimation()
{
	// ����K�w�̃��f���`��R���|�[�l���g��T���ĂƂ肠�����ݒ�
	SetModelComponent(GetGameObject()->GetComponent<KdModelRendererComponent>());

	// �ǂݍ��񂾃��f���f�[�^�̑SAnimation�f�[�^��ۑ�
	std::string filepath;
	if (KdEditorData::GetInstance().OpenFileDialog(filepath))
	{
		auto model = KdResourceManager::GetInstance().LoadAsset<KdModel>(filepath);

		// �t�H���_���̂ݔ����o��
		std::filesystem::path fs(filepath);
		std::string directoryPath = fs.parent_path().string();

		// �Ǝ��̃A�j���[�V�����f�[�^�Ƃ��ĕۑ�
		for(auto anim : model->GetAnimations())
		{
			anim.Save(directoryPath);
		}
	}

}

// �A�j���[�V�����t�@�C�����w�肵�ēǂݍ���
void KdAnimationComponent::LoadFromAnimationDataWithDialog()
{
	// �A�j���[�V�������ɕK�v�ȃf�[�^�̏���
	std::string selectFilePath;
	if (KdEditorData::GetInstance().OpenFileDialog(selectFilePath, u8"�A�j���[�V�����f�[�^���J��", "*.kdanim"))
	{
		LoadFromAnimationData(selectFilePath);
	}
}

KdAnimationComponent::RuntimeAnimationData& KdAnimationComponent::LoadFromAnimationData(const std::string& path)
{
	// AnimationData��ǂݍ���
	auto anim = KdResourceManager::GetInstance().LoadAsset<KdAnimationData>(path);

	// AnimationData�Ɗ֘A�t�������s���f�[�^���쐬
	RuntimeAnimationData runtime;
	runtime.AniamtionData = anim;
	runtime.AnimationDataPath = path;
	runtime.AnimationName = anim->Name; // �Ǘ����͍ŏ��̓t�@�C����
	m_runtimeDatas.push_back(runtime);

	// ���łɃ��f��������΃A�j���[�V�����f�[�^�Ɠ˂����킹�Ă���
	auto modelComp = m_modelComponent.lock();
	if(modelComp && modelComp->GetModel() != nullptr)
	{
		runtime.CreateApplyIndexList(modelComp->GetModel()->GetAllNodes());
	}

	// �������������v�f��Ԃ�
	return *(m_runtimeDatas.end() - 1);
}

//KdAnimationComponent::RuntimeAnimationData& KdAnimationComponent::LoadFromAnimationData(const std::string& path, const float speed, const float startTime)
//{
//	// AnimationData��ǂݍ���
//	auto anim = KdResourceManager::GetInstance().LoadAsset<KdAnimationData>(path);
//
//	// AnimationData�Ɗ֘A�t�������s���f�[�^���쐬
//	RuntimeAnimationData runtime;
//	runtime.AniamtionData = anim;
//	runtime.AnimationDataPath = path;
//	runtime.AnimationName = anim->Name; // �Ǘ����͍ŏ��̓t�@�C����
//	runtime.speed = speed;
//	runtime.StartTime = startTime;
//	m_runtimeDatas.push_back(runtime);
//
//	// ���łɃ��f��������΃A�j���[�V�����f�[�^�Ɠ˂����킹�Ă���
//	auto modelComp = m_modelComponent.lock();
//	if (modelComp && modelComp->GetModel() != nullptr)
//	{
//		runtime.CreateApplyIndexList(modelComp->GetModel()->GetAllNodes());
//	}
//
//	// �������������v�f��Ԃ�
//	return *(m_runtimeDatas.end() - 1);
//}


// �A�j���[�V������K�p�����郂�f���R���|�[�l���g�̎w��
void KdAnimationComponent::SetModelComponent(std::shared_ptr<KdModelRendererComponent> modelComp)
{
	if(modelComp == nullptr){return;}
	if(modelComp->GetModel() == nullptr){return;}

	m_modelComponent = modelComp;
	auto model = modelComp->GetSourceModel();
	if(model == nullptr){return;}

	// �v�Z���ʂ��i�[����̈�m��
	m_calculatedTransform = std::make_shared< std::vector<KdTransform> >();
	if(m_calculatedTransform)
	{
		model->CreateNodeTransforms(*m_calculatedTransform);
	}

	// �K�p�C���f�b�N�X���X�g�쐬
	for(auto&& anim : m_runtimeDatas)
	{
		anim.CreateApplyIndexList(model->GetAllNodes());
	}
}


// �A�j���[�V���������������Ď��s
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
			// �N���X�t�F�[�h�̎��Ԃ��ݒ肳��Ă���(����Ă��Ă��Đ����̃A�j���[�V�������Ȃ���Α����ړ�)
			if (crossFadeTime > 0 && IsPlaying())
			{
				anim.ease = std::make_shared<KdEase>();
				anim.ease->Start(KdEase::Type::Straight, crossFadeTime, false);

				// ���ݍĐ����̃A�j���[�V�����Ƀt�F�[�h�A�E�g����������ł���
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
				Stop(); // ���̃A�j���[�V�����͒�~���đ����Ɉړ�
			}

			// �A�j���[�V�����Đ��ݒ�
			anim.isEnable	= true;
			anim.isPlaying	= true;
			anim.isLoop		= isLoop;
			anim.weight		= 1.0f;

			//��������Đ����邩
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
			// �N���X�t�F�[�h�̎��Ԃ��ݒ肳��Ă���(����Ă��Ă��Đ����̃A�j���[�V�������Ȃ���Α����ړ�)
			if (crossFadeTime > 0 && IsPlaying())
			{
				anim.ease = std::make_shared<KdEase>();
				anim.ease->Start(KdEase::Type::Straight, crossFadeTime, false);

				// ���ݍĐ����̃A�j���[�V�����Ƀt�F�[�h�A�E�g����������ł���
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
				Stop(); // ���̃A�j���[�V�����͒�~���đ����Ɉړ�
			}

			// �A�j���[�V�����Đ��ݒ�
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
	// �A�j���[�V�������w�茟��
	if(animationName != "")
	{
		for (auto&& runtime : m_runtimeDatas)
		{
			if (runtime.AnimationName == animationName && runtime.isPlaying) { return true; }
		}
		return false;
	}

	// �S����
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
		// �A�j���[�V�����f�[�^�̓t�@�C�������o���Ă��邾���Ȃ̂ŁA�ēx�쐬
		std::string path;
		KdJsonUtility::GetValue(arrJson.at(idx), "AnimationDataPath", &path);
		auto runtime = LoadFromAnimationData(path);

		// ���s���f�[�^�̕���
		KdJsonUtility::GetValue(arrJson.at(idx), "AnimationName", &runtime.AnimationName);

		//���g�Œǉ���������
		//�A�j���[�V�������x
		KdJsonUtility::GetValue(arrJson.at(idx), "AnimationSpeed", &runtime.speed);
		//�A�j���[�V���������J�n����
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

		//���g�Œǉ���������
		obj["AnimationSpeed"] = anim.speed;
		obj["AnimationStartTime"] = anim.StartTime;
		
		arr.push_back(obj);
	}
	outJson["AnimationDatas"] = arr;
}

void KdAnimationComponent::Editor_ImGui()
{	
	
	KdComponent::Editor_ImGui();

	// �Ǘ����Ă���A�j���[�V�����ꗗ
	if (ImGui::CollapsingHeader(u8"�A�j���[�V�����ꗗ", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for(auto&& anim : m_runtimeDatas)
		{
			if(anim.AniamtionData == nullptr){continue;}
			ImGui::PushID(&anim);
			if (ImGui::CollapsingHeader(anim.AnimationName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::InputText( u8"�Ǘ���", &anim.AnimationName);
				ImGui::LabelText(std::to_string(anim.AniamtionData->LengthInSecond).c_str(), u8"���Đ�����");
				
				//�A�j���[�V���������L�����N�^�[�ɓK�����邩
				if(anim.isEnable)
				{
					ImGui::LabelText(std::to_string(anim.weight).c_str(), u8"�K�p��");
				}

				if (anim.isPlaying)
				{
					ImGui::LabelText( std::to_string(anim.ProgressTime).c_str(), u8"�Đ���");
				}

				if (ImGui::Button(u8"Play"))
				{
					Play(anim.AnimationName, false, 3.0f);
				}

				//�A�j���[�V�������x����
				ImGui::DragFloat(u8"�i�s���x", &anim.speed, 0.01f, 0.0f, 100.0f);

				//�A�j���[�V������������
				ImGui::DragFloat(u8"��������", &anim.StartTime, 0.01f, 0.0f, 1.0f);


			}
			ImGui::PopID();
		}
	}

	// �t�@�C���̓ǂݍ���
	if (ImGui::CollapsingHeader(u8"�t�@�C���ǂݍ���", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::Button(u8"���f���f�[�^����A�j���[�V�����f�[�^�փR���o�[�g"))
		{
			ConvertModelToAnimation();
		}

		if(ImGui::Button(u8"�A�j���[�V�����f�[�^����ǂݍ���"))
		{
			LoadFromAnimationDataWithDialog();
		}

		ImGui::Unindent();
	}
}


// �L���ɂȂ��Ă���A�j���[�V�������X�V���ĕ⊮���ʂ��v�Z����
bool KdAnimationComponent::RuntimeAnimationData::UpdateAnimation(std::shared_ptr<KdModelRendererComponent> modelComp, std::vector<InterpolatedResult>& results)
{
	
	if (isEnable == false) { return false; }

	auto& modelData = modelComp->GetSourceModel();

	bool isEnd = false;

	auto& time = KdTime::GetInstance();

	// ���Ԃ̍X�V�ƃ��[�v
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

	// �t�F�[�hIN OUT
	if(ease)
	{
		weight = ease->NextValue();
		if(ease->IsEnd())
		{
			ease = nullptr;
			// �t�F�[�h���I����Ă��Ă��Aweight��0��������A�j���[�V�������I������Ɣ��f
			if(weight <= 0.01f)
			{
				isPlaying = false;
				isEnable = false;
			}
		}
	}

	// �i�s���0-1��
	double progressPer = ProgressTime / AniamtionData->LengthInSecond;

	// ����m�F�p
	int useFrame = (int)(progressPer * 60);

	// �m�[�h���ɕ⊮
	for (auto&& perNode : AniamtionData->PerNodes)
	{
		const std::string& nodeName = perNode.Name; // �A�j���[�V����������m�[�h��

		// �Ǘ����郂�f���R���|�[�l���g�ɊY������m�[�h�����邩
		auto node = modelData->GetNode(nodeName);
		//if (node == nullptr) { continue; }

		// �e�v�f�̕��
		auto interpolation = AniamtionData->Interpolation(perNode, ProgressTime);

		// ���ʂ��X�^�b�N
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

		// �����^�C�����o�������
		for(UINT idx = 0; idx < AniamtionData->PerNodes.size(); idx++)
		{
			if (modelNode.NodeName == AniamtionData->PerNodes[idx].Name)
			{
				ApplyNodeIndex[idx] = modelNode.NodeIndex;
				hit = true;
			}
		}

		// �A�j���[�V�����f�[�^�ɖ��ߍ��ޕ������p�~�\��
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







