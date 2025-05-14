#include"KdFramework.h"
#include"Collision.h"

#include"Gravity.h"
#include"ObjectList.h"

//フレームワークにコンポーネントであることを登録
SetClassAssembly(Collision, "Component");

bool Collision::RayCast(const KdVector3& startPos, const KdVector3& direction)
{
	return false;
}

void Collision::Start()
{
	m_spGravity = GetGameObject()->GetComponent<Gravity>();
	m_spObjList = GetGameObject()->GetComponent<ObjectList>();
}

void Collision::Update()
{
	//有効無効フラグチェック
	if (IsEnable() == false) { return; }

	if (m_spObjList == nullptr) { return; }
	//全ての有効(Enable)なオブジェクトを取得する
	//auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();

	//Component持ち主のTransform情報とposition情報を取得
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetPosition();

	m_wpTarget.reset();

	for (auto& ray : m_rayDatas)
	{
		//有効無効判定
		if (ray.isEnable == false) { continue; }

		//レイ判定---------------------->
		KdVector3 vStartPos = pos;

		//重力加算
		float fallDist = 0;
		if (ray.isGravity)
		{
			if (m_spGravity) { 

			//	座標移動は行われていると仮定して移動する前分ずらす
			fallDist = -m_spGravity->GetMoveOnes().y;
			if (fallDist<0)
			{
 				fallDist = 0;
			}
			}
		}
		
		vStartPos += ray.correction ;	//プレイヤーの乗り越えられる段差の高さ
		vStartPos.y += fallDist;		//重力をyに加算


		//無理やりのやり方をスマートに変える必要あり
		if (ray.list == Check::Ground) {
			auto& enableObjects = m_spObjList->GetGrounds();

			//キャラクター全員と当たり判定
			for (auto&& obj : enableObjects)
			{
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

					if (ray.isGravity)
					{
						bool hit = KdRayToMesh(vStartPos, ray.direction, ray.correction.y + fallDist, *node.Mesh, oppMat, hitResult);

						//
						if (hit)
						{
							m_spGravity->Ground();
							pos.y += (ray.correction.y + fallDist) - hitResult.Distance;
							transform->SetPosition(pos);

							if (ray.isTarget == true)
							{
								//対象オブジェクトの生ポインタをweak_ptrに格納
								std::shared_ptr<KdGameObject> spObj(obj);
								m_wpTarget = spObj;
							//	m_wpTargetb = obj;
							}
						}
					}
					else
					{
						bool hit = KdRayToMesh(vStartPos, ray.direction, ray.correction.y, *node.Mesh, oppMat, hitResult);

						if (hit)
						{
							//当たった判定位置から差し戻す距離を引いた分をセットする
							pos.y += ray.correction.y - hitResult.Distance;
							transform->SetPosition(pos);


							if (ray.isTarget == true)
							{
								//対象オブジェクトの生ポインタをweak_ptrに格納
								std::shared_ptr<KdGameObject> spObj(obj);
								m_wpTarget = spObj;
							//	m_wpTargetb = obj;

							}

						}
					}


				}
			}

		}

		if (ray.list == Check::Player)
		{
			auto& enableObjects = m_spObjList->GetPlayers();

			//キャラクター全員と当たり判定
			for (auto&& obj : enableObjects)
			{
			
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

					if (ray.isGravity)
					{
						bool hit = KdRayToMesh(vStartPos, ray.direction, ray.correction.y + fallDist, *node.Mesh, oppMat, hitResult);

						//
						if (hit)
						{
							m_spGravity->Ground();
							pos.y += (ray.correction.y + fallDist) - hitResult.Distance;
							transform->SetPosition(pos);


							if (ray.isTarget == true)
							{
								//対象オブジェクトの生ポインタをweak_ptrに格納
								std::shared_ptr<KdGameObject> spObj(obj);
								m_wpTarget = spObj;
							//	m_wpTargetb = obj;
							}
						}
					}
					else
					{
						bool hit = KdRayToMesh(vStartPos, ray.direction, ray.correction.y, *node.Mesh, oppMat, hitResult);

						if (hit)
						{
							//当たった判定位置から差し戻す距離を引いた分をセットする
							pos.y += ray.correction.y - hitResult.Distance;
							transform->SetPosition(pos);


							if (ray.isTarget == true)
							{
								//対象オブジェクトの生ポインタをweak_ptrに格納
								std::shared_ptr<KdGameObject> spObj(obj);
								m_wpTarget = spObj;
							//	m_wpTargetb = obj;

							}

						}
					}


				}
			}
		}

		if (ray.list == Check::Enemy)
		{
			auto& enableObjects = m_spObjList->GetEnemys();

			//キャラクター全員と当たり判定
			for (auto&& obj : enableObjects)
			{
		
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

					if (ray.isGravity)
					{
						bool hit = KdRayToMesh(vStartPos, ray.direction, ray.correction.y + fallDist, *node.Mesh, oppMat, hitResult);

						//
						if (hit)
						{
							m_spGravity->Ground();
							pos.y += (ray.correction.y + fallDist) - hitResult.Distance;
							transform->SetPosition(pos);


							if (ray.isTarget == true)
							{
								//対象オブジェクトの生ポインタをweak_ptrに格納
								std::shared_ptr<KdGameObject> spObj(obj);
								m_wpTarget = spObj;
							//	m_wpTargetb = obj;

							}
						}
					}
					else
					{
						bool hit = KdRayToMesh(vStartPos, ray.direction, ray.correction.y, *node.Mesh, oppMat, hitResult);

						if (hit)
						{
							//当たった判定位置から差し戻す距離を引いた分をセットする
							pos.y += ray.correction.y - hitResult.Distance;
							transform->SetPosition(pos);


							if (ray.isTarget == true)
							{
								//対象オブジェクトの生ポインタをweak_ptrに格納
								std::shared_ptr<KdGameObject> spObj(obj);
								m_wpTarget = spObj;
							//	m_wpTargetb = obj;

							}

						}
					}


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

		//プッシュバック初期化
		sphere.vPushBack ={ 0, 0, 0};

		//地形
		if (sphere.list == Check::Ground) {
			auto& enableObjects = m_spObjList->GetGrounds();
			//前キャラクターの繰り返し
			for (auto&& obj : enableObjects)
			{
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

					if (hit && sphere.isResult)
					{
						//移動した結果から元の位置を引く 差し戻し分を累積
						sphere.vPushBack += (vMove - vCenter);
					}

					if (sphere.isTarget == true)
					{
						//対象オブジェクトの生ポインタをweak_ptrに格納
						std::shared_ptr<KdGameObject> spObj(obj);
						m_wpTarget = spObj;
					//	m_wpTargetb = obj;

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

		//プレイヤー
		if (sphere.list == Check::Player) {
			auto& enableObjects = m_spObjList->GetPlayers();
			//前キャラクターの繰り返し
			for (auto&& obj : enableObjects)
			{


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

					if (hit && sphere.isResult)
					{
						//移動した結果から元の位置を引く 差し戻し分を累積
						sphere.vPushBack += (vMove - vCenter);
					}

					if (sphere.isTarget == true)
					{
						//対象オブジェクトの生ポインタをweak_ptrに格納
						std::shared_ptr<KdGameObject> spObj(obj);
						m_wpTarget = spObj;
					//	m_wpTargetb = obj;

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

		//敵
		if (sphere.list == Check::Enemy) {
			auto& enableObjects = m_spObjList->GetEnemys();
			//前キャラクターの繰り返し
			for (auto&& obj : enableObjects)
			{

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

					if (hit && sphere.isResult)
					{
						//移動した結果から元の位置を引く 差し戻し分を累積
						sphere.vPushBack += (vMove - vCenter);
					}

					if (sphere.isTarget == true)
					{
						//対象オブジェクトの生ポインタをweak_ptrに格納
						std::shared_ptr<KdGameObject> spObj(obj);
						m_wpTarget = spObj;
					//	m_wpTargetb = obj;

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

				//重力の影響を受けるかどうか
				ImGui::Checkbox(u8"重力", &ray.isGravity);

				//当たった対象を取得するか
				ImGui::Checkbox(u8"対象取得", &ray.isTarget);

				//当たり判定をするリスト
				ImGui::RadioButton(u8"地形", &ray.list, Check::Ground);
				ImGui::RadioButton(u8"プレイヤー", &ray.list, Check::Player);
				ImGui::RadioButton(u8"敵", &ray.list, Check::Enemy);

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

				//当たった対象を取得するか
				ImGui::Checkbox(u8"対象取得", &sphere.isTarget);

				//当たり判定をするリスト
				ImGui::RadioButton(u8"地形", &sphere.list, Check::Ground);
				ImGui::RadioButton(u8"プレイヤー", &sphere.list, Check::Player);
				ImGui::RadioButton(u8"敵", &sphere.list, Check::Enemy);

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

void Collision::CollisionRay()
{

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
		obj["Name"] = ray.Name;
		obj["Enable"] = ray.isEnable;
		obj["Gravity"] = ray.isGravity;
		obj["Target"] = ray.isTarget;
		obj["List"] = ray.list;
		obj["StartPos"]= KdJsonUtility::CreateArray(&ray.startPos.x, 3);
		obj["Correction"]= KdJsonUtility::CreateArray(&ray.correction.x, 3);
		obj["Direction"] = KdJsonUtility::CreateArray(&ray.direction.x, 3);
		arr.push_back(obj);
	}
	outJson["RayDatas"] = arr;
	auto arrs = nlohmann::json::array();
	for (auto& sphere : m_sphereDatas)
	{	
		if (m_sphereDatas.size() == 0) { break; }

		nlohmann::json obj;
		obj["Name"] = sphere.Name;
		obj["Enable"] = sphere.isEnable;
		obj["Result"] = sphere.isResult;
		obj["Target"] = sphere.isTarget;
		obj["List"] = sphere.list;
		obj["Center"] = KdJsonUtility::CreateArray(&sphere.center.x, 3);
		obj["Radius"] = sphere.radius;
		arrs.push_back(obj);
	}
	outJson["SphereDatas"] = arrs;
	
}

void Collision::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	auto arrJson = jsonObj["RayDatas"];
	for (UINT idx = 0; idx < (UINT)arrJson.size(); idx++)
	{
		RayColInfo ray;

		// 実行時データの復元
		KdJsonUtility::GetValue(arrJson.at(idx), "Name", &ray.Name);
		KdJsonUtility::GetValue(arrJson.at(idx), "Enable", &ray.isEnable);
		KdJsonUtility::GetValue(arrJson.at(idx), "Gravity", &ray.isGravity);
		KdJsonUtility::GetValue(arrJson.at(idx), "Target", &ray.isTarget);
		KdJsonUtility::GetValue(arrJson.at(idx), "List", &ray.list);
		KdJsonUtility::GetArray(arrJson.at(idx), "StartPos", &ray.startPos.x,3);
		KdJsonUtility::GetArray(arrJson.at(idx), "Correction", &ray.correction.x,3);
		KdJsonUtility::GetArray(arrJson.at(idx), "Direction", &ray.direction.x,3);

		m_rayDatas.push_back(ray);
	}
	arrJson = jsonObj["SphereDatas"];
	for (UINT idx = 0; idx < (UINT)arrJson.size(); idx++)
	{
		SphereColInfo sphere;

		// 実行時データの復元
		KdJsonUtility::GetValue(arrJson.at(idx), "Name", &sphere.Name);
		KdJsonUtility::GetValue(arrJson.at(idx), "Enable", &sphere.isEnable);
		KdJsonUtility::GetValue(arrJson.at(idx), "Result", &sphere.isResult);
		KdJsonUtility::GetValue(arrJson.at(idx), "Target", &sphere.isTarget);
		KdJsonUtility::GetValue(arrJson.at(idx), "List", &sphere.list);
		KdJsonUtility::GetArray(arrJson.at(idx), "Center", &sphere.center.x, 3);
		KdJsonUtility::GetValue(arrJson.at(idx), "Radius", &sphere.radius );
		sphere.vPushBack = {0,0,0};
		
		m_sphereDatas.push_back(sphere);
	}
}
