#include"KdEffectComponent.h"

//KdPolygonの中身include
#include"0_App/Shader/Polygon/KdPolygon.h"
//描画するのに必要
#include"../KdFramework.h"

SetClassAssembly(KdEffectComponent, "Component");

void KdEffectComponent::Start()
{
	//ポリゴン作成
	m_poly = std::make_shared<KdPolygon>();
	m_poly->Initialize();

	//ポリゴンの形を作成	(1m*1m固定変更時はscaleをいじる)
	m_poly->AddVertex(KdVector3(-0.5f, -0.5f, 0), KdVector2(0.0f, 1.0f), 0xFFFFFFFF);		//左下
	m_poly->AddVertex(KdVector3(-0.5f, 0.5f, 0), KdVector2(0.0f, 0.0f), 0xFFFFFFFF);		//左上
	m_poly->AddVertex(KdVector3(0.5f, -0.5f, 0), KdVector2(1.0f, 1.0f), 0xFFFFFFFF);		//右下
	m_poly->AddVertex(KdVector3(0.5f, 0.5f, 0), KdVector2(1.0f, 0.0f), 0xFFFFFFFF);		//右上

	//テクスチャを仮のものを読み込んでおく
	m_poly->SetTexture("./Assets/Textures/Flare.png");

}

void KdEffectComponent::Update()
{

}

void KdEffectComponent::LateUpdate()
{

}


void KdEffectComponent::PreDraw()
{
	//このオブジェクトが有効か
	if (IsEnable() == false) { return; }

	//ビルボード処理
	
	// このGameObjectのTransform
	const auto& trans = GetGameObject()->GetTransform();

	// カメラの情報
	auto camera = KdFramework::GetInstance().m_renderingData.m_currentScreenData->m_camera;

	//カメラの行列
	auto camMat = camera->GetCameraMatrix();
	//回転成分だけを抽出
	camMat._41 = camMat._42 = camMat._43 = 0;

	// GameObjectの位置情報
	auto pos = trans->GetLocalPosition();
	KdMatrix posMat = KdMatrix::CreateTranslation(pos);

	//追加のZ回転角度
	KdMatrix zRotMat = KdMatrix::CreateRotationZ(m_zRotation * KdDeg2Rad);

	//追加のスケール
	KdVector3 scale = { m_scale.x,m_scale.y,1.0f };
	KdMatrix scaleMat = KdMatrix::CreateScale(scale);

	//行列合成
	KdMatrix totalMat = scaleMat* zRotMat * camMat  * posMat;

	trans->SetLocalMatrix(totalMat);



	//描画対象として登録
	KdFramework::GetInstance().m_renderingData.m_currentScreenData->m_drawList.push_back(this);



}

void KdEffectComponent::Draw(bool opaque, KdShader::PassTags passTag)
{
	if (m_poly == nullptr) { return; }	//描画対象が居ない

	// 不透明物描画で無い時(透明物である)
	if (opaque == false) { return; }

	//影要描画の時
	if (passTag == KdShader::PassTags::ShadowCaster) { return; }
	
	//シェーダー
	auto& sm = KdShaderManager::GetInstance();

	// GameObjectの場所に描画
	sm.m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();

	sm.m_cbPerDraw->WriteWorkData();	//シェーダーに送り込む

	// 加算合成にする
	sm.m_bs_Add->SetToDevice();

	//描画
	m_poly->Draw();

	//アルファブレンドに戻す
	sm.m_bs_Alpha->SetToDevice();

}

void KdEffectComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//回転角度
	ImGui::DragFloat(u8"回転角度", &m_zRotation, 0.1f, 0.0f, 360.0f);
	ImGui::DragFloat2(u8"拡縮", &m_scale.x, 0.1f, 0.1f, 10.0f);
	
}

void KdEffectComponent::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);
	
	KdJsonUtility::GetValue(jsonObj, "zRotate", &m_zRotation);
	KdJsonUtility::GetArray(jsonObj, "Scale", &m_scale.x, 2);

}

void KdEffectComponent::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["zRotate"] = m_zRotation;
	outJson["Scale"] = KdJsonUtility::CreateArray(&m_scale.x, 2);
}