#include"KdMovableObject.h"

#include"KdFramework.h"

SetClassAssembly(KdMovableObject, "Component");

void KdMovableObject::Start()
{
	Set();
}

void KdMovableObject::Update()
{
	if (m_enable == false) { return; }

	if (m_moveObject.expired() == true) { return; }

	if (m_wayPoints.size() < 2) { return;}

	if (m_wayNum > m_wayPoints.size() - 2)
	{
		//
		m_wayNum = m_wayPoints.size() - 2;
	}
	
	m_start = m_wayPoints[m_wayNum];
	m_end = m_wayPoints[m_wayNum+1];

	// �e�I�u�W�F�N�g�̊m��
	auto moveObj = m_moveObject.lock();
	auto startObj = m_start.lock();
	auto endObj = m_end.lock();


	//���݂̐i�s��̌v�Z
	float p = m_duration / (m_oneMoveTime / m_wayPoints.size() -1 ); // 0-1
	float wp = m_waitduration / m_waitTime;

	static bool endFlg = false;

	//�������Ȃ��ꍇ
	if (m_roundTrip == false)
	{
		if (p >= 1.0f)
		{
			if (wp >= 1.0f)
			{
				p = 0.0f;
				m_duration = 0.0f;
				m_waitduration = 0.0f;
				m_wayNum += 1;
			}
			else
			{
				m_waitduration += KdTime::GetInstance().GetDeltaTime();
			}
		}
		else
		{
			//�o�ߎ��ԍX�V
			m_duration += KdTime::GetInstance().GetDeltaTime();
		}
	}
	// ��������ꍇ
	else
	{
		if (endFlg == false)
		{
			if (p >= 1.0f)
			{
				if (wp >= 1.0f)
				{	
					m_waitduration = 0.0f;
					endFlg = true;
					m_wayNum += 1;
				}
				else
				{
					m_waitduration += KdTime::GetInstance().GetDeltaTime();
				}
			}
			else
			{
				//�o�ߎ��ԍX�V
				m_duration += KdTime::GetInstance().GetDeltaTime();
			}
		}
		else
		{
			if (p <= 0.0f)
			{
				if (wp >= 1.0f)
				{
					p = 0.0f;
					m_duration = 0.0f;
					m_waitduration = 0.0f;
					m_wayNum -= 1;
					endFlg = false;
				}
				else
				{
					m_waitduration += KdTime::GetInstance().GetDeltaTime();
				}
			}
			else
			{
				//�o�ߎ��ԍX�V
				m_duration -= KdTime::GetInstance().GetDeltaTime();
			}
		}
	}

	if (m_duration > 1.0f)
	{
		m_duration = 1.0f;
	}

	// Transform�̎擾
	auto tT = moveObj->GetTransform();
	auto sT = startObj->GetTransform();
	auto eT = endObj->GetTransform();

	// �e�v�f��ʁX�Ɍv�Z----------------------------------->
	// �ړ��⊮
	auto sP = sT->GetLocalPosition();
	auto eP = eT->GetLocalPosition();
//	auto Pos = KdVector3::Lerp(sP, eP, m_duration);
	auto Pos = sP + (eP - sP) * p;
	tT->SetLocalPosition(Pos);

	// �g�k�⊮
	auto sS = sT->GetLocalScale();
	auto eS = eT->GetLocalScale();
	auto Scale = sS + (eS - sS) * p;
	tT->SetLocalScale(Scale);

	// ��]�⊮
	auto sR = sT->GetLocalRotation();
	auto eR = eT->GetLocalRotation();
	auto Rotate = KdQuaternion::Slerp(sR, eR, p);
	tT->SetLocalRotation(Rotate);

	// <-----------------------------------�e�v�f��ʁX�Ɍv�Z
	

}

void KdMovableObject::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	// �����I�u�W�F�N�g
	if (m_moveObject.expired() == false)
	{
		auto move = m_moveObject.lock();
		ImGui::LabelText(u8"�����I�u�W�F�N�g",u8"%s\n",move->GetName().c_str());

	}
	
	// �X�^�[�g�n�_
	if (m_start.expired() == false)
	{
		auto start = m_start.lock();
		auto vSP = start->GetTransform()->GetLocalPosition();
		ImGui::LabelText(u8"�J�n�n�_", u8"Name : %s,x :%f,y :%f,z : %f\n", start->GetName().c_str(), vSP.x, vSP.y, vSP.z);
	}

	// �G���h�n�_
	if (m_end.expired() == false)
	{
		auto end = m_end.lock();
		auto vSP = end->GetTransform()->GetLocalPosition();
		ImGui::LabelText(u8"�I���n�_", u8"Name : %s,x :%f,y :%f,z : %f\n", end->GetName().c_str(), vSP.x, vSP.y, vSP.z);
	}

	// �Ȃ��Ȃ���
	if (ImGui::Button(u8"�|�C���g�̍Đݒ�")) { Set(); }

	ImGui::DragFloat(u8"�Г��K�v����", &m_oneMoveTime, 0.01f);
	ImGui::DragFloat(u8"�҂�����", &m_waitTime, 0.01f);
	ImGui::Checkbox(u8"�������邩", &m_roundTrip);
}

void KdMovableObject::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["OneMoveTime"] = m_oneMoveTime;
	outJson["WaitTime"] = m_waitTime;
	outJson["RoundTrip"] = m_roundTrip;
}

void KdMovableObject::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "OneMoveTime", &m_oneMoveTime);
	KdJsonUtility::GetValue(jsonObj, "WaitTime", &m_waitTime);
	KdJsonUtility::GetValue(jsonObj, "RoundTrip", &m_roundTrip);
}



void KdMovableObject::Set()
{
	// �e���ڃN���A
	m_duration = 0.0f;
	m_moveObject.reset();
	m_start.reset();
	m_end.reset();
	m_wayPoints.clear();

	// �q�ǂ������̎擾
	auto children = GetGameObject()->GetChildren();
	if (children.size() <= 2) { return; }

	// 1�l�� = �����Ώ�
	m_moveObject = children.front();
	children.pop_front();

	//2�l�߈ȍ~��waypoint�Ɋi�[
	for (auto& child : GetGameObject()->GetChildren())
	{
		m_wayPoints.push_back(child);
	}

}
