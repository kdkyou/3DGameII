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

	m_vMoveOnce = KdVector3::Zero;

	if (keyboard.W)
	{
		m_vMoveOnce.z = 1.0f;
	}
	if (keyboard.A)
	{
		m_vMoveOnce.x = -1.0f;
	}
	if (keyboard.S)
	{
		m_vMoveOnce.z = -1.0f;
	}
	if (keyboard.D)
	{
		m_vMoveOnce.x = 1.0f;
	}

	//正規化
	m_vMoveOnce.Normalize();
	m_vMoveOnce = m_vMoveOnce * m_speed * deltaTime;

	//ジャンプ
	if (keyboard.Space)
	{
		m_fall = -m_jumpPow;
	}

	//落下
	m_vMoveOnce.y -= m_fall;
	m_fall += m_gravity * deltaTime;


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
		}

		//強パンチ(アニメーション遅め)
		if (keyboard.L)
		{
			m_animation->Play("Attack", 11.0f);
		}

	}


	//当たり判定
	CollisionPhase();

}

void Player::LateUpdate()
{

}

void Player::CollisionPhase()
{
	//全ての有効(Enable)なオブジェクトを取得する
	auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();

	//PlayerのTransform情報とposition情報を取得
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetPosition();

	//下向きのレイ判定(着地判定)---------------------->
	float fallDist = -m_vMoveOnce.y;//	一回の落下量
	if (fallDist < 0) {
		fallDist = 0;
	}

	KdVector3 vStartPos = pos;
	vStartPos.y += m_rayCorrection	//プレイヤーの乗り越えられる段差の高さ
		+ fallDist;			//一回の落下量も加算しておく

	//キャラクター全員と当たり判定
	for (auto&& obj : enableObjects)
	{
		//当たり判定の対象かどうか
		if (obj->GetTag() != "CollisionObject") { continue; }

		//対象がモデルデータを持っているか GetComponentの後ろに型を指定する 
		// #define等で型の設定をしている方が変更があった時しやすいか？
		const auto& modelComp = obj->GetComponent<KdModelRendererComponent>();
		if (modelComp == nullptr) { continue; }

		//モデルの持ち主のTransformの取得
		const auto& modelTrans = modelComp->GetGameObject()->GetTransform();

		//モデル情報の取得
		const auto& modelData = modelComp->GetModel();
		if (modelData == nullptr) { continue; }

		//モデルに対して当たり判定
		for (auto& node : modelData->GetAllNodes())
		{
			//モデルに関する全ての情報を持っている為
			//メッシュかどうかを調べる
			if (node.Mesh == nullptr) { continue; }

			//レイ判定開始
			KdRayHit hitResult; //結果格納場所

			//各パーツの原点からのズレ分
			const auto& meshMat = modelComp->GetAllNodeTransforms()[node.NodeIndex].GetWorldMatrix();
			//各パーツの現在の位置
			auto oppMat = meshMat * modelTrans->GetWorldMatrix();

			//メッシュに対する当たり判定
			bool hit = KdRayToMesh(vStartPos, KdVector3(0, -1, 0), m_rayCorrection + fallDist, *node.Mesh, oppMat, hitResult);

			if (hit)
			{
				//当たった判定位置からはみ出た分で差し戻す距離をセットする
				pos.y += (m_rayCorrection + fallDist) - hitResult.Distance;
				transform->SetPosition(pos);
				m_fall = 0.0f;
			}
		}
	}
	//<----------------------下向きのレイ判定(着地判定)


	//横向きの当たり判定(着地判定)---------------------->
	// 球体メッシュの当たり判定
	KdVector3 vCenter = pos;	//球の中心座標
	vCenter.y += m_sphereCorrection;	//少しもち上げる
	KdVector3 vPushBack = {};	//差し戻されるベクトルの合計

	//前キャラクターの繰り返し
	for (auto&& obj : enableObjects)
	{
		//当たり判定対象か
		if (obj->GetTag() != "CollisionObject") { continue; }

		//対象のモデル情報
		const auto& modelComp = obj->GetComponent<KdModelRendererComponent>();
		if (modelComp == nullptr) { continue; }

		//対象のモデルデータ
		const auto& modelData = modelComp->GetModel();
		if (modelData == nullptr) { continue; }

		for (auto& node : modelData->GetAllNodes())
		{
			if (node.Mesh == nullptr) { continue; }

			auto mat = modelComp->GetAllNodeTransforms()[node.NodeIndex].GetWorldMatrix()
				* obj->GetTransform()->GetWorldMatrix();

			//球判定
			KdSphereHit hitResult;
			KdVector3 vMove;	//差し戻された移動量

			hitResult.MovedSpherePos = &vMove;

			bool hit = KdSphereToMesh(vCenter, m_sphereRadius, *node.Mesh, mat, hitResult);

			if (hit)
			{
				//移動した結果から元の位置を引く 差し戻し分を累積
				vPushBack += (vMove - vCenter);
			}
		}

	}
	//上下の制限はレイ判定で行っているので不要
	vPushBack.y = 0;
	if (vPushBack.Length() > 0)
	{
		pos += vPushBack;
		transform->SetPosition(pos);
	}

	//<----------------------横向きの当たり判定(着地判定)

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

	//ジャンプ力調整
	ImGui::DragFloat(
		u8"ジャンプ力",
		&m_jumpPow,
		0.01f,
		0.0f,
		100.0f
	);

	//重力調整
	ImGui::DragFloat(
		u8"重力",
		&m_gravity,
		0.001f,
		0.0f,
		10.0f
	);

	//当たり判定の補正値
	ImGui::DragFloat(
		u8"レイ補正値",
		&m_rayCorrection,
		0.001f,
		0.0f,
		10.0f
	);

	ImGui::DragFloat(
		u8"スフィア補正値",
		&m_sphereCorrection,
		0.001f,
		0.0f,
		1.0f
	);

	ImGui::DragFloat(
		u8"スフィア半径",
		&m_sphereRadius,
		0.01f,
		0.0f,
		100.0f
	);

}

//Playerで必要なデータの保存
void Player::Serialize(nlohmann::json& outJson) const
{
	//保存が必要なデータを[文字列]-[データ]のセットで覚える

	// 基底クラスの関数も実行しておく
	KdComponent::Serialize(outJson);

	outJson["Speed"] = m_speed;
	outJson["Gravity"] = m_gravity;
	outJson["JumpPow"] = m_jumpPow;
	/*outJson["RayCorrection"] = m_rayCorrection;
	outJson["SphereCorrection"] = m_sphereCorrection;
	outJson["SphereRadius"] = m_sphereRadius;*/

	//outJson["Speed"]=KdJsonUtility::CreateArray(&m_speed, 1);
}

void Player::Deserialize(const nlohmann::json& jsonObj)
{
	//保存されたデータをファイルから呼び出し
	//データに格納させる
	//継承元の関数も実行
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "Speed", &m_speed);
	KdJsonUtility::GetValue(jsonObj, "Gravity", &m_gravity);
	KdJsonUtility::GetValue(jsonObj, "JumpPow", &m_jumpPow);
	/*KdJsonUtility::GetValue(jsonObj, "RayCorrection", &m_rayCorrection);
	KdJsonUtility::GetValue(jsonObj, "SphereCorrection", &m_sphereCorrection);
	KdJsonUtility::GetValue(jsonObj, "SphereRadius", &m_sphereRadius);*/
}

