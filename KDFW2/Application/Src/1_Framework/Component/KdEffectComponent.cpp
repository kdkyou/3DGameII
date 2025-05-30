#include"KdEffectComponent.h"

//KdPolygonの中身include
#include"0_App/Shader/Polygon/KdPolygon.h"
//描画するのに必要
#include"../KdFramework.h"
//アセット関係に必要
#include"Editor/KdEditorData.h"

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

	
	if (m_playWithStart == true)
	{
		StartAnim();
	}

}

void KdEffectComponent::Update()
{

	if (m_isStarted == false) { return; }

	m_scale = KdVector2::Lerp(m_startScale, m_endScale, m_duration);

	float time =m_ease.NextValue();

	m_duration = time;

	if (m_duration > m_lifetime) {
		m_duration = m_lifetime;
		m_isStarted = false;
	}

}

void KdEffectComponent::LateUpdate()
{

}


void KdEffectComponent::PreDraw()
{
	//このオブジェクトが有効か
	if (IsEnable() == false) { return; }

	//現在、Draw前にi回のみ判定しているが、Drawはカメラ回数分回る

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
	KdMatrix scaleMat = KdMatrix::CreateScale(m_scale.x,m_scale.y,1.0f);

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
	m_poly->Draw(KdPolygon::DrawType::fill);

	//アルファブレンドに戻す
	sm.m_bs_Alpha->SetToDevice();

}

void KdEffectComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//回転角度
	ImGui::DragFloat(u8"回転角度", &m_zRotation, 0.1f, 0.0f, 360.0f);

	//現在のテクスチャパス
	ImGui::LabelText(m_textureFilePath.c_str(), u8"表示テクスチャ\n");

	//使用するテクスチャの選択
	if (ImGui::Button(u8"テクスチャ変更"))
	{
		std::string selectFilePath = "";
		if (KdEditorData::GetInstance().OpenFileDialog(
			selectFilePath, "画像データの選択"))
		{
			//画像データとして読み込めるかどうか
			auto tex = KdResourceManager::GetInstance().LoadAsset<KdTexture>(selectFilePath);
			if (tex != nullptr)
			{
				m_texture = tex;
				m_textureFilePath = selectFilePath;
				//テクスチャを読み込む
				m_poly->SetTexture(tex);
			}
		}
		

	}
	
	//XY拡縮
	ImGui::DragFloat2(u8"拡縮", &m_scale.x, 0.05f, 0.0f, 10.0f);

	//アニメーション再生時間
	ImGui::DragFloat(u8"時間", &m_lifetime, 0.01f, 0.1f, 100.0f);

	//初期の拡縮率
	ImGui::DragFloat2(u8"初期拡縮率", &m_startScale.x, 0.01f, 0.0f, 100.0f);

	//終了時の拡縮率
	ImGui::DragFloat2(u8"終了拡縮率", &m_endScale.x, 0.01f, 0.0f, 100.0f);

	//生成時に再生するか
	ImGui::Checkbox(u8"生成再生", &m_playWithStart);

	//再生ボタン
	if (ImGui::Button(u8"Play"))
	{
		StartAnim();

	}
	
}

void KdEffectComponent::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);
	
	KdJsonUtility::GetValue(jsonObj, "zRotate", &m_zRotation);
	KdJsonUtility::GetArray(jsonObj, "Scale", &m_scale.x, 2);
	KdJsonUtility::GetValue(jsonObj, "Lifetime", &m_lifetime);
	KdJsonUtility::GetArray(jsonObj, "StartScale", &m_startScale.x, 2);
	KdJsonUtility::GetArray(jsonObj, "EndScale", &m_endScale.x, 2);
	KdJsonUtility::GetValue(jsonObj, "PlayWith", &m_playWithStart);
	KdJsonUtility::GetValue(jsonObj, "TexturePath", &m_textureFilePath);
}

void KdEffectComponent::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["zRotate"] = m_zRotation;
	outJson["Scale"] = KdJsonUtility::CreateArray(&m_scale.x, 2);
	outJson["Lifetime"] = m_lifetime;
	outJson["StartScale"] = KdJsonUtility::CreateArray(&m_startScale.x, 2);
	outJson["EndScale"] = KdJsonUtility::CreateArray(&m_endScale.x, 2);
	outJson["PlayWith"] = m_playWithStart;
	outJson["TexturePath"] = m_textureFilePath;

}

void KdEffectComponent::StartAnim()
{
	m_isStarted = true;
	m_duration = 0.0f;
	m_ease.Start(KdEase::OutSine, m_lifetime);
}
