#include"KdFramework.h"
#include"Enemy.h"

#include"Collision.h"

//フレームワークにコンポーネントであることを登録
SetClassAssembly(Enemy, "Component");

void Enemy::Start()
{
	m_spCollision = GetGameObject()->
		GetComponent<Collision>();
}

void Enemy::Update()
{
	if (IsEnable() == false) { return; }

	if (m_spCollision == nullptr) { return; }

	std::shared_ptr<KdGameObject> spObj = m_spCollision->GetTarget().lock();
//	auto spObj = m_spCollision->GetTargetb();

	if (spObj == nullptr) { return; }

	//自身の位置情報を取得
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetLocalPosition();

	//対象の位置情報を取得
	const auto& targetTrans = spObj->GetTransform();
	auto targetPos = targetTrans->GetLocalPosition();

	//差分を計算
	auto difference = targetPos - pos;

	//自身のべクトル
	auto nowDir = transform->GetWorldMatrix().Backward();

	//対象位置のベクトル
	auto vec = difference;
	vec.Normalize();


	//角度算出
	float nowAng = atan2(nowDir.x, nowDir.z);
	nowAng = DirectX::XMConvertToDegrees(nowAng);

	float targetAng = atan2(vec.x, vec.z);
	targetAng = DirectX::XMConvertToDegrees(targetAng);

	float betweenAng = targetAng - nowAng;
	if (betweenAng > 180)
	{
		betweenAng -= 360;
	}
	else if (betweenAng < -180)
	{
		betweenAng += 360;
	}
	
	float rotateAng = std::clamp(betweenAng, -m_ang, m_ang);

	float deltaTime = KdTime::GetInstance().GetDeltaTime();

	//現在の回転情報
	KdQuaternion quat = transform->GetLocalRotation();

	KdQuaternion qAdd = KdQuaternion::CreateFromAxisAngle(
		KdVector3(0, 1, 0),					//回転の中心軸	外積がいいだろう
		rotateAng * KdDeg2Rad * deltaTime		//回転角度(ラジアン角度)
	);

	//回転を適用				加算分だけ増やしたい場合掛け算
	transform->SetLocalRotation(quat * qAdd);

	//y座標の差分を考慮しない
	difference.y = 0;
	
	//差分が一定以上の時移動処理開始
	if (difference.Length() >= m_difference && rotateAng<1.0f)
	{	
		//正面方向のベクトル取得
		KdVector3 vf = transform->GetWorldMatrix().Backward();

		KdVector3 vMoveOnce = vf * m_speed * deltaTime;

		pos += vMoveOnce;

		transform->SetLocalPosition(pos);

	}


}

void Enemy::LateUpdate()
{

}


void Enemy::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["Speed"] = m_speed;
	outJson["Angle"] = m_ang;
	outJson["Difference"] = m_difference;
}

void Enemy::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "Speed", &m_speed);
	KdJsonUtility::GetValue(jsonObj, "Angle", &m_ang);
	KdJsonUtility::GetValue(jsonObj, "Difference", &m_difference);
}

void Enemy::Editor_ImGui()
{
	//基底クラスの関数を呼び出し
	//有効無効チェック
	KdComponent::Editor_ImGui();

	//移動スピードの調整
	ImGui::DragFloat(
		u8"速度",		//表示名
		&m_speed,		//関連付ける変数ポインタ
		0.01f,			//一回の更新量
		0.0f,			//最小値
		100.0f			//最大値
	);

	//回転角度
	ImGui::DragFloat(
		u8"角度",		//表示名
		&m_ang,		//関連付ける変数ポインタ
		0.01f,			//一回の更新量
		0.0f,			//最小値
		100.0f			//最大値
	);

	//維持距離
	ImGui::DragFloat(
		u8"維持距離",		//表示名
		&m_difference,		//関連付ける変数ポインタ
		0.01f,			//一回の更新量
		0.0f,			//最小値
		100.0f			//最大値
	);
}
