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

	// 各オブジェクトの確保
	auto moveObj = m_moveObject.lock();
	auto startObj = m_start.lock();
	auto endObj = m_end.lock();


	//現在の進行具合の計算
	float p = m_duration / (m_oneMoveTime / m_wayPoints.size() -1 ); // 0-1
	float wp = m_waitduration / m_waitTime;

	static bool endFlg = false;

	//往復しない場合
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
			//経過時間更新
			m_duration += KdTime::GetInstance().GetDeltaTime();
		}
	}
	// 往復する場合
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
				//経過時間更新
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
				//経過時間更新
				m_duration -= KdTime::GetInstance().GetDeltaTime();
			}
		}
	}

	if (m_duration > 1.0f)
	{
		m_duration = 1.0f;
	}

	// Transformの取得
	auto tT = moveObj->GetTransform();
	auto sT = startObj->GetTransform();
	auto eT = endObj->GetTransform();

	// 各要素を別々に計算----------------------------------->
	// 移動補完
	auto sP = sT->GetLocalPosition();
	auto eP = eT->GetLocalPosition();
//	auto Pos = KdVector3::Lerp(sP, eP, m_duration);
	auto Pos = sP + (eP - sP) * p;
	tT->SetLocalPosition(Pos);

	// 拡縮補完
	auto sS = sT->GetLocalScale();
	auto eS = eT->GetLocalScale();
	auto Scale = sS + (eS - sS) * p;
	tT->SetLocalScale(Scale);

	// 回転補完
	auto sR = sT->GetLocalRotation();
	auto eR = eT->GetLocalRotation();
	auto Rotate = KdQuaternion::Slerp(sR, eR, p);
	tT->SetLocalRotation(Rotate);

	// <-----------------------------------各要素を別々に計算
	

}

void KdMovableObject::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	// 動くオブジェクト
	if (m_moveObject.expired() == false)
	{
		auto move = m_moveObject.lock();
		ImGui::LabelText(u8"動くオブジェクト",u8"%s\n",move->GetName().c_str());

	}
	
	// スタート地点
	if (m_start.expired() == false)
	{
		auto start = m_start.lock();
		auto vSP = start->GetTransform()->GetLocalPosition();
		ImGui::LabelText(u8"開始地点", u8"Name : %s,x :%f,y :%f,z : %f\n", start->GetName().c_str(), vSP.x, vSP.y, vSP.z);
	}

	// エンド地点
	if (m_end.expired() == false)
	{
		auto end = m_end.lock();
		auto vSP = end->GetTransform()->GetLocalPosition();
		ImGui::LabelText(u8"終了地点", u8"Name : %s,x :%f,y :%f,z : %f\n", end->GetName().c_str(), vSP.x, vSP.y, vSP.z);
	}

	// つなぎなおし
	if (ImGui::Button(u8"ポイントの再設定")) { Set(); }

	ImGui::DragFloat(u8"片道必要時間", &m_oneMoveTime, 0.01f);
	ImGui::DragFloat(u8"待ち時間", &m_waitTime, 0.01f);
	ImGui::Checkbox(u8"往復するか", &m_roundTrip);
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
	// 各項目クリア
	m_duration = 0.0f;
	m_moveObject.reset();
	m_start.reset();
	m_end.reset();
	m_wayPoints.clear();

	// 子どもたちの取得
	auto children = GetGameObject()->GetChildren();
	if (children.size() <= 2) { return; }

	// 1人め = 動く対象
	m_moveObject = children.front();
	children.pop_front();

	//2人め以降はwaypointに格納
	for (auto& child : GetGameObject()->GetChildren())
	{
		m_wayPoints.push_back(child);
	}

}
