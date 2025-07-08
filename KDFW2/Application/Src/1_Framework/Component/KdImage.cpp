#include"KdImage.h"

#include"0_App/Shader/Polygon/KdPolygon.h"
#include"KdFramework.h"
#include"Editor/KdEditorData.h"

SetClassAssembly(KdImage, "Component");

void KdImage::Start()
{
	m_polygon = std::make_shared<KdPolygon>();
	m_polygon->Initialize();
	
	//Guidが設定されていたら(読み込んでいたら)テクスチャを読み込む
	if (m_imageGuid != "" && m_texture == nullptr)
	{
		// Guidからファイルパスの取得
		auto path = KdAssetManager::GetInstance().GetFilePathWithGuid(m_imageGuid);
		m_texture = KdResourceManager::GetInstance().LoadAsset<KdTexture>(path);

	}
	CreateVertex();	// デフォルトの表示用頂点を作る



}

void KdImage::Update()
{
	// Zは0で固定する
	auto pos = GetGameObject()->GetTransform()->GetLocalPosition();

	pos.z = 0.0f;

	GetGameObject()->GetTransform()->SetLocalPosition(pos);
}

void KdImage::PreDraw()
{
	if (m_enable == false) { return; }

	KdFramework::GetInstance().m_renderingData.m_currentScreenData->m_drawList.push_back(this);

}

// 各タイミングで呼び出される
void KdImage::Draw(bool opaque, KdShader::PassTags passTag)
{
	if (passTag != KdShader::PassTags::Sprite) { return; }

	auto& sm = KdShaderManager::GetInstance();

	// Image本体のTransform
	sm.m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();
	sm.m_cbPerDraw->WriteWorkData();

	//// カメラは単位行列にする
	//sm.m_cbPerCamera->EditCB().mV = KdMatrix::Identity;

	//// 射影変換行列はピクセル→射影空間へ変換する行列
	//auto mStoP = KdMatrix::CreateOrthographic(1920, 1080, 0, 1);
	//sm.m_cbPerCamera->EditCB().mP =mStoP ;

	//// 行列の更新
	//sm.m_cbPerCamera->WriteWorkData();

	// 描画
	m_polygon->Draw();
}

void KdImage::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	// テクスチャの読み込み →将来的に廃止
	if (ImGui::Button(u8"画像の読み込み"))
	{
		std::string filePath = "";
		if (KdEditorData::GetInstance().OpenFileDialog(
			filePath, "画像データの選択"))
		{
			//画像データとして読み込めるかどうか
			auto tex = KdResourceManager::GetInstance().LoadAsset<KdTexture>(filePath);

			if (tex == nullptr) { return; }

			// 初めてテクスチャを設定したのであれば
			// 画像の幅高さにサイズを合わせる
			if (m_texture == nullptr)
			{
				m_width = tex->GetWidth();
				m_height = tex->GetHeight();
			}

			m_texture = tex;
			
			CreateVertex();
		}
	}

	// Guidを指定してテクスチャを選択
	auto selAsset = KdAssetManager::GetInstance().GetSelectedAsset();

	if (selAsset != nullptr)
	{
		// データタイプの検査(テクスチャかどうか)

		// 選択中のAssetをImageとして使う
		std::string bStr = selAsset->FileName + u8"を設定";
		if (ImGui::Button(bStr.c_str()))
		{
			

			//画像データとして読み込めるかどうか
			auto tex = KdResourceManager::GetInstance().LoadAsset<KdTexture>(selAsset->FilePath);

			if (tex == nullptr) { return; }

			// AssetのGuidを覚えておく
			m_imageGuid = selAsset->guid;

			// 初めてテクスチャを設定したのであれば
			// 画像の幅高さにサイズを合わせる
			if (m_texture == nullptr)
			{
				m_width = tex->GetWidth();
				m_height = tex->GetHeight();
			}

			m_texture = tex;

			CreateVertex();

		}
	}

	// 使用中の画像名
	std::string imageName = "default";
	auto asset = KdAssetManager::GetInstance().GetAssetPropertyWithGuid(m_imageGuid);
	// Guidからファイル情報が取れた
	if (asset != nullptr) { imageName = asset->FileName; }
	// Guidが設定されているのに、ファイル情報が見つからなかった
	else if (m_imageGuid != "") { imageName = "missing"; }
	// 表示
	ImGui::LabelText(u8"使用画像", imageName.c_str());



	// ピクセル座標の調整
	bool ch = false;
	auto pos = GetGameObject()->GetTransform()->GetLocalPosition();
	ch |= ImGui::DragFloat2(u8"ピクセル移動", &pos.x, 1.0f, -5000.0f, 5000.0f);
	if (ch == true)
	{
		GetGameObject()->GetTransform()->SetLocalPosition(pos);
	}

	ch = false;

	ch |= ImGui::DragInt(u8"横幅", &m_width);
	ch |= ImGui::DragInt(u8"縦幅", &m_height);
	if(ch ==true)
	{
		CreateVertex();
	}
	

}

void KdImage::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "useGuid", &m_imageGuid);
	auto pos = GetGameObject()->GetTransform()->GetLocalPosition();
	KdJsonUtility::GetArray(jsonObj, "Pos", &pos.x, 2);
	KdJsonUtility::GetValue(jsonObj, "Width", &m_width);
	KdJsonUtility::GetValue(jsonObj, "Height", &m_height);

	GetGameObject()->GetTransform()->SetLocalPosition(pos);

}

void KdImage::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["useGuid"] = m_imageGuid;
	auto pos = GetGameObject()->GetTransform()->GetLocalPosition();
	outJson["Pos"] =KdJsonUtility::CreateArray(&pos.x,2);
	outJson["Width"] = m_width;
	outJson["Height"] = m_height;

}


void KdImage::CreateVertex()
{
	m_polygon->Release();

	// ピクセル単位で頂点を作る
	float pw = (float)m_width / 2.0f;
	float ph = (float)m_height / 2.0f;

	//ポリゴンの形を作成	(1m*1m固定変更時はscaleをいじる)
	m_polygon->AddVertex(KdVector3(-pw, -ph, 0), KdVector2(0.0f, 1.0f), 0xFFFFFFFF);		//左下
	m_polygon->AddVertex(KdVector3(-pw, ph, 0), KdVector2(0.0f, 0.0f), 0xFFFFFFFF);		//左上
	m_polygon->AddVertex(KdVector3(pw, -ph, 0), KdVector2(1.0f, 1.0f), 0xFFFFFFFF);		//右下
	m_polygon->AddVertex(KdVector3(pw, ph, 0), KdVector2(1.0f, 0.0f), 0xFFFFFFFF);		//右上

	if (m_texture == nullptr) {
	// 白テクスチャを描画
	 m_polygon->SetTexture(D3D.GetDotWhiteTex());
	}
	else {
	// 画像があれば画像
	 m_polygon->SetTexture(m_texture);
	}
}
