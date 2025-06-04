#include"KdFramework.h"
#include"Player.h"

//フレームワークにコンポーネントであることを登録
SetClassAssembly(Player, "Component");

void Player::Start()
{
	m_animation = GetGameObject()->
		GetComponent<KdAnimationComponent>();
}

void Player::Update()
{
	//有効無効フラグチェック
	if (IsEnable() == false) { return; }



	//PlayerのTransfoemを取得
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetLocalPosition();

	//入力検知
	auto& keyboard = KdInput::GetInstance().GetKeyboardState();
	auto& mouse = KdInput::GetInstance().GetMouseState();

	//float speed = 0.01f;	//1フレームに動かしたい移動量 ※旧
	//float speed = 1.0f;	//１秒間に動かしたい量

	//１秒間の量→1フレームの量
	float deltaTime = KdTime::GetInstance().GetDeltaTime();

	//２つの数値の乗算で量を決める

	//キーボードの動きによって前後左右の移動
	//下向きに落下する処理&ジャンプの処理
	//今の所、字面はy=0の位置とする

	//キャラクターの前方向
	KdVector3 vf = transform->GetWorldMatrix().Backward();
	KdVector3 vr = transform->GetWorldMatrix().Right();
	KdVector3 vl = transform->GetWorldMatrix().Left();
	KdVector3 vb = transform->GetWorldMatrix().Forward();

	vf.Normalize();
	vr.Normalize();
	vl.Normalize();
	vb.Normalize();

	m_vMoveOnce = KdVector3::Zero;

	if (keyboard.W)
	{
		m_vMoveOnce = vf * m_speed * deltaTime;
	}
	/*if (keyboard.W)
	{
		m_vMoveOnce.z = 1.0f;
	}*/
	if (keyboard.A)
	{
		m_vMoveOnce = vl * m_speed * deltaTime;
	}
	if (keyboard.S)
	{
		m_vMoveOnce = vb * m_speed * deltaTime;
	}
	if (keyboard.D)
	{
		m_vMoveOnce = vr * m_speed * deltaTime;
	}

	//正規化
	m_vMoveOnce.Normalize();
	m_vMoveOnce = m_vMoveOnce * m_speed * deltaTime;

	// 回転
	//現在の回転情報
	KdQuaternion quat = transform->GetLocalRotation();

	//加算したい回転
	KdQuaternion qAdd = KdQuaternion::CreateFromAxisAngle(
		KdVector3(0,1,0),					//回転の中心軸	外積がいいだろう
		20.0f*KdDeg2Rad * deltaTime		//回転角度(ラジアン角度)
	);

	//回転を適用				加算分だけ増やしたい場合掛け算
	transform->SetLocalRotation(quat * qAdd);


	////ジャンプ
	//if (keyboard.Space)
	//{
	//	m_fall = -m_jumpPow;
	//}

	////落下
	//m_vMoveOnce.y -= m_fall;
	//m_fall += m_gravity * deltaTime;

	pos += m_vMoveOnce;

	//仮地面判定
	/*if (pos.y <- 0.5f)
	{
		pos.y =- 0.5f;
		m_fall = 0.0f;
	}*/

	//更新結果をTransformに教える
	transform->SetLocalPosition(pos);

	//アニメーションが存在しているかどうか
	if (m_animation == nullptr) { return; }

	//攻撃		ゲットコンポーネントは重い処理なのでUpdateでしない
	if (m_animation->IsPlaying("Attack") == false)
	{
		//弱パンチ(アニメーション早め)
		if (keyboard.K)
		{
			m_animation->Play("Attack", 22.0f);

			//エフェクト発生!!
			auto effect = KdFramework::GetInstance().GetScene()->Instantiate(
				"./Assets/Prefabs/Slash.kdprefab",
				nullptr		//エフェクトは置いてくる
			);
			auto pos = transform->GetLocalPosition();
			pos.y += 1.0f;
			pos.x += 0.3f;

			//エフェクトをプレイヤーの位置に移動
			effect->GetTransform()->SetLocalPosition(pos);
		}
	}


	//当たり判定
//	CollisionPhase();

}

void Player::LateUpdate()
{

}

void Player::Editor_ImGui()
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

}

//Playerで必要なデータの保存
void Player::Serialize(nlohmann::json& outJson) const
{
	//保存が必要なデータを[文字列]-[データ]のセットで覚える

	// 基底クラスの関数も実行しておく
	KdComponent::Serialize(outJson);

	outJson["Speed"] = m_speed;

}

void Player::Deserialize(const nlohmann::json& jsonObj)
{
	//保存されたデータをファイルから呼び出し
	//データに格納させる
	//継承元の関数も実行
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "Speed", &m_speed);
}

