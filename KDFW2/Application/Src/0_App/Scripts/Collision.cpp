#include"KdFramework.h"
#include"Collision.h"

//フレームワークにコンポーネントであることを登録
SetClassAssembly(Collision, "Component");

void Collision::Start()
{

}

void Collision::Update()
{
	//有効無効フラグチェック
	if (IsEnable() == false) { return; }

	//全ての有効(Enable)なオブジェクトを取得する
	auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();

	//Component持ち主のTransform情報とposition情報を取得
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetPosition();

	for (auto& ray : m_rayDatas)
	{
		//有効無効判定
		if (ray.isEnable == false) { continue; }

		//レイ判定---------------------->
		KdVector3 vStartPos = pos;
		vStartPos += ray.correction;	//プレイヤーの乗り越えられる段差の高さ

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
				bool hit = KdRayToMesh(vStartPos, ray.direction, ray.correction.y, *node.Mesh, oppMat, hitResult);

				if (hit)
				{
					//当たった判定位置から差し戻す距離を引いた分をセットする
					pos.y += ray.correction.y - hitResult.Distance;
					transform->SetPosition(pos);

				}
			}
		}
	}
	//<----------------------下向きのレイ判定(着地判定)

	for (auto& sphere : m_sphereDatas)
	{
		//横向きの当たり判定(着地判定)---------------------->
		// 球体メッシュの当たり判定
		KdVector3 vCenter = pos;	//球の中心座標
		vCenter += sphere.center;	//少しもち上げる
		
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

				bool hit = KdSphereToMesh(vCenter, sphere.radius, *node.Mesh, mat, hitResult);

				if (hit&&sphere.isResult)
				{
					//移動した結果から元の位置を引く 差し戻し分を累積
					sphere.vPushBack += (vMove - vCenter);
				}
			}

		}
		//上下の制限はレイ判定で行っているので不要
		sphere.vPushBack.y = 0;
		if (sphere.vPushBack.Length() > 0)
		{
			pos += sphere.vPushBack;
			transform->SetPosition(pos);
		}
	}
	//<----------------------横向きの当たり判定(着地判定)

}

void Collision::LateUpdate()
{
}

void Collision::Editor_ImGui()
{
	//有効無効チェック
	KdComponent::Editor_ImGui();

	// 管理しているレイ判定一覧
	if (ImGui::CollapsingHeader(u8"レイ判定一覧", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto&& ray : m_rayDatas)
		{
		ImGui::PushID(&ray);
			if (ImGui::CollapsingHeader(ray.Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				//何判定か管理できるように名前
				ImGui::InputText(u8"管理名", &ray.Name);
				
				//判定の有効無効
				ImGui::Checkbox(u8"有効無効",&ray.isEnable);

				//レイの始める位置
				ImGui::DragFloat3(u8"開始地点", &ray.startPos.x, 0.01f, -1.0f, 1.0f);

				//レイの補正値
				ImGui::DragFloat3(u8"許容値", &ray.correction.x, 0.01f, 0.0f, 10.0f);
				
				//レイの方向ベクトル
				ImGui::DragFloat3(u8"ベクトル", &ray.direction.x, 0.01f, -1.0f, 1.0f);
			}
		ImGui::PopID();
		}
	}

	// 管理しているスフィア判定一覧
	if (ImGui::CollapsingHeader(u8"スフィア判定一覧", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto&& sphere : m_sphereDatas)
		{
			ImGui::PushID(&sphere);
			if (ImGui::CollapsingHeader(sphere.Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				//何判定か管理できるように名前
				ImGui::InputText(u8"管理名", &sphere.Name);

				//判定の有効無効
				ImGui::Checkbox(u8"有効無効", &sphere.isEnable);

				//判定結果の格納するかしないか
				ImGui::Checkbox(u8"ベクトル格納", &sphere.isResult);

				//スフィアのセンター位置
				ImGui::DragFloat3(u8"センター", &sphere.center.x, 0.01f, -1.0f, 1.0f);

				//スフィアの半径
				ImGui::DragFloat(u8"半径", &sphere.radius, 0.01f, 0.0f, 100.0f);

			}
		ImGui::PopID();
		}
	}

	if(ImGui::Button(u8"レイ追加"))
	{
		AddRay();
	}

	if (ImGui::Button(u8"スフィア追加"))
	{
		AddSphere();
	}

}

void Collision::AddRay()
{
	RayColInfo ray;
	ray.Name = "ray";
	m_rayDatas.push_back(ray);
}

void Collision::AddSphere()
{
	SphereColInfo sphere;
	sphere.Name = "sphere";
	m_sphereDatas.push_back(sphere);
}

void Collision::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	//判定データを保存
	auto arr = nlohmann::json::array();
	for (auto&& ray : m_rayDatas)
	{
		nlohmann::json obj;
		obj["Enable"] = ray.isEnable;
		/*obj["RayStartPos"]= ray.startPos;
		obj["RayCorrection"]= ray.correction;
		obj["RayDirection"] = ray.direction;
		*/obj["RayName"] = ray.Name;
		arr.push_back(obj);
	}
	for (auto& sphere : m_sphereDatas)
	{
		nlohmann::json obj;
		obj["Enable"] = sphere.isEnable;
		obj["Result"] = sphere.isResult;
		/*obj["SphereCenter"] = sphere.center;
		*/obj["SphereRadius"] = sphere.radius;
		obj["SphereName"] = sphere.Name;
		arr.push_back(obj);
	}
	outJson["CollisionDatas"] = arr;
	



}

void Collision::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);


}


//void Collision::CollisionPhase()
//{
//	//全ての有効(Enable)なオブジェクトを取得する
//	auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();
//
//	//PlayerのTransform情報とposition情報を取得
//	const auto& transform = GetGameObject()->GetTransform();
//	auto pos = transform->GetPosition();
//
//	//下向きのレイ判定(着地判定)---------------------->
//	KdVector3 vStartPos = pos;
//	vStartPos.y += m_rayCorrection;	//プレイヤーの乗り越えられる段差の高さ
//
//	//キャラクター全員と当たり判定
//	for (auto&& obj : enableObjects)
//	{
//		//当たり判定の対象かどうか
//		if (obj->GetTag() != "CollisionObject") { continue; }
//
//		//対象がモデルデータを持っているか GetComponentの後ろに型を指定する 
//		// #define等で型の設定をしている方が変更があった時しやすいか？
//		const auto& modelComp = obj->GetComponent<KdModelRendererComponent>();
//		if (modelComp == nullptr) { continue; }
//
//		//モデルの持ち主のTransformの取得
//		const auto& modelTrans = modelComp->GetGameObject()->GetTransform();
//
//		//モデル情報の取得
//		const auto& modelData = modelComp->GetModel();
//		if (modelData == nullptr) { continue; }
//
//		//モデルに対して当たり判定
//		for (auto& node : modelData->GetAllNodes())
//		{
//			//モデルに関する全ての情報を持っている為
//			//メッシュかどうかを調べる
//			if (node.Mesh == nullptr) { continue; }
//
//			//レイ判定開始
//			KdRayHit hitResult; //結果格納場所
//
//			//各パーツの原点からのズレ分
//			const auto& meshMat = modelComp->GetAllNodeTransforms()[node.NodeIndex].GetWorldMatrix();
//			//各パーツの現在の位置
//			auto oppMat = meshMat * modelTrans->GetWorldMatrix();
//
//			//メッシュに対する当たり判定
//			bool hit = KdRayToMesh(vStartPos, KdVector3(0, -1, 0), m_rayCorrection + m_fall, *node.Mesh, oppMat, hitResult);
//
//			if (hit)
//			{
//				//当たった判定位置から差し戻す距離を引いた分をセットする
//				pos.y += m_rayCorrection - hitResult.Distance;
//				transform->SetPosition(pos);
//				m_fall = 0.0f;
//			}
//		}
//	}
//	//<----------------------下向きのレイ判定(着地判定)
//
//
//	//横向きの当たり判定(着地判定)---------------------->
//	// 球体メッシュの当たり判定
//	KdVector3 vCenter = pos;	//球の中心座標
//	vCenter.y += m_sphereCorrection;	//少しもち上げる
//	KdVector3 vPushBack = {};	//差し戻されるベクトルの合計
//
//	//前キャラクターの繰り返し
//	for (auto&& obj : enableObjects)
//	{
//		//当たり判定対象か
//		if (obj->GetTag() != "CollisionObject") { continue; }
//
//		//対象のモデル情報
//		const auto& modelComp = obj->GetComponent<KdModelRendererComponent>();
//		if (modelComp == nullptr) { continue; }
//
//		//対象のモデルデータ
//		const auto& modelData = modelComp->GetModel();
//		if (modelData == nullptr) { continue; }
//
//		for (auto& node : modelData->GetAllNodes())
//		{
//			if (node.Mesh == nullptr) { continue; }
//
//			auto mat = modelComp->GetAllNodeTransforms()[node.NodeIndex].GetWorldMatrix()
//				* obj->GetTransform()->GetWorldMatrix();
//
//			//球判定
//			KdSphereHit hitResult;
//			KdVector3 vMove;	//差し戻された移動量
//
//			hitResult.MovedSpherePos = &vMove;
//
//			bool hit = KdSphereToMesh(vCenter, m_sphereRadius, *node.Mesh, mat, hitResult);
//
//			if (hit)
//			{
//				//移動した結果から元の位置を引く 差し戻し分を累積
//				vPushBack += (vMove - vCenter);
//			}
//		}
//
//	}
//	//上下の制限はレイ判定で行っているので不要
//	vPushBack.y = 0;
//	if (vPushBack.Length() > 0)
//	{
//		pos += vPushBack;
//		transform->SetPosition(pos);
//	}
//
//	//<----------------------横向きの当たり判定(着地判定)
//
//}