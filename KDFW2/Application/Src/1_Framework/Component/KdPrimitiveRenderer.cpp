#include"KdPrimitiveRenderer.h"

//6/2追加
#include"0_APP/Shader/Polygon/KdPolygon.h"
#include"KdFrameWork.h"

//描画のエディタ画面
#include"Editor/KdEditorData.h"

//フレームワークにコンポーネントであることを登録
SetClassAssembly(KdPrimitiveRenderer, "Component");


bool KdPrimitiveRenderer::LineShape::CreateVertex(std::shared_ptr<KdPolygon> poly)
{
	//6/2追加
	if (poly == nullptr) { return false; }

	//ラインになっているかどうか
	if (IsEstablished() == false) { return false; }
	//すでに最新の状況
	if (m_isCreated == true) { return false; }

	//頂点の作成-------------------->
	poly->Release();	//ポリゴンの作り直し

	//一番最初の点 = 原点(0,0,0)スタート
	poly->AddVertex(KdVector3(0, 0, 0), KdVector2(0, 0), m_color);

	//二点目移行は存在するもの全部
	for (auto& p : m_points)
	{
		poly->AddVertex(p, KdVector2(0, 0), m_color);
	}
	//<-------------------->頂点の作成

	m_isCreated = true;		///最新の情報になったフラグ

	return true;
}

bool KdPrimitiveRenderer::LineShape::IsEstablished()
{
	//線として成立していない
	if (m_points.size() <= 0) { return false; }

	//↓trueに
	return true;
}

void KdPrimitiveRenderer::LineShape::Editor_ImGui()
{
	//現在のポイント一覧
	if (ImGui::TreeNode("points"))
	{
		int cnt = 0;
		for (auto& p : m_points)
		{
			ImGui::Text("%f,%f,%f", p.x, p.y, p.z);
		}
		ImGui::TreePop();
	}

	//ポイントの追加
	static KdVector3 vDir = { 0,0,1 };
	static float length = 1.0f;
	ImGui::DragFloat3(u8"伸ばす方向", &vDir.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"伸ばす距離", &length, 0.01f, 0.0f);

	if (ImGui::Button(u8"新しいポイント作成"))
	{
		AddPoint(vDir, length);
	}

}

UINT KdPrimitiveRenderer::LineShape::AddPoint(const KdVector3& dir, float length)
{
	//新しいポイントを登録してよいか
	if (dir.Length() <= 0.0f) { return (UINT)m_points.size(); }
	if (length <= 0) { return (UINT)m_points.size(); }

	//開始地点 = 登録された最後のポイント
	KdVector3 vStart = { 0,0,0 };
	if (m_points.size() > 0)
	{
		vStart = *(m_points.end() - 1);		//登録された最後の要素
	}

	//追加する場所を計算
	auto dire = dir;
	dire.Normalize();
	KdVector3 vEnd = vStart + (dire * length);
	m_points.push_back(vEnd);

	//次の描画で頂点の作り直し
	ReCreate();


	return (UINT)m_points.size();
}


// 球体描画
bool KdPrimitiveRenderer::SphereShape::CreateVertex(std::shared_ptr<KdPolygon> poly)
{
	//6/2追加
	if (poly == nullptr) { return false; }

	//ラインになっているかどうか
	if (IsEstablished() == false) { return false; }
	//すでに最新の状況
	if (m_isCreated == true) { return false; }

	if (m_detail <= 0) { return false; }

	//頂点の作成-------------------->
	poly->Release();	//ポリゴンの作り直し

	KdVector3 pos;

	//posをcenterではなくposに変えたりすると変化がある

	for (auto& s : m_spheres)
	{

		int count =0;

		for (int i = 0; i < m_detail; ++i)
		{

			//　XZ面
			pos = s.center;
			pos.x += cos((float)i * (360 / m_detail) * KdDeg2Rad) * s.radius;
			pos.z += sin((float)i * (360 / m_detail) * KdDeg2Rad) * s.radius;
			poly->AddVertex(pos, KdVector2(0, 0), m_color);

			pos = s.center;
			pos.x += cos((float)(i + 1) * (360 / m_detail) * KdDeg2Rad) * s.radius;
			pos.z += sin((float)(i + 1) * (360 / m_detail) * KdDeg2Rad) * s.radius;
			poly->AddVertex(pos, KdVector2(0, 0), m_color);

		}
		for (int i = 0; i < m_detail; ++i)
		{
			if (i > m_detail / 4) { 
				count = i;
				break; }
			// XY面
			pos = s.center;
			pos.x += cos((float)i * (360 / m_detail) * KdDeg2Rad) * s.radius;
			pos.y += sin((float)i * (360 / m_detail) * KdDeg2Rad) * s.radius;
			poly->AddVertex(pos, KdVector2(0, 0), m_color);

			pos = s.center;
			pos.x += cos((float)(i+1) * (360 / m_detail) * KdDeg2Rad) * s.radius;
			pos.y += sin((float)(i + 1) * (360 / m_detail) * KdDeg2Rad) * s.radius;
			poly->AddVertex(pos, KdVector2(0, 0), m_color);
		}
		
		for (int i = 0; i < m_detail; ++i)
		{

			// YZ面
			pos = s.center;
			pos.y += cos((float)i * (360 / m_detail) * KdDeg2Rad) * s.radius;
			pos.z += sin((float)i * (360 / m_detail) * KdDeg2Rad) * s.radius;
			poly->AddVertex(pos, KdVector2(0, 0), m_color);

			pos = s.center;
			pos.y += cos((float)(i + 1) * (360 / m_detail) * KdDeg2Rad) * s.radius;
			pos.z += sin((float)(i + 1) * (360 / m_detail) * KdDeg2Rad) * s.radius;
			poly->AddVertex(pos, KdVector2(0, 0), m_color);
		}

		for (int i = count; i < m_detail; ++i)
		{
			// XY面
			pos = s.center;
			pos.x += cos((float)i * (360 / m_detail) * KdDeg2Rad) * s.radius;
			pos.y += sin((float)i * (360 / m_detail) * KdDeg2Rad) * s.radius;
			poly->AddVertex(pos, KdVector2(0, 0), m_color);

			pos = s.center;
			pos.x += cos((float)(i + 1) * (360 / m_detail) * KdDeg2Rad) * s.radius;
			pos.y += sin((float)(i + 1) * (360 / m_detail) * KdDeg2Rad) * s.radius;
			poly->AddVertex(pos, KdVector2(0, 0), m_color);

		}

	}
	//<-------------------->頂点の作成

	m_isCreated = true;		///最新の情報になったフラグ

	return true;
}

bool KdPrimitiveRenderer::SphereShape::IsEstablished()
{
	//線として成立していない
	if (m_spheres.size() <= 0) { return false; }

	//↓trueに
	return true;
}

void KdPrimitiveRenderer::SphereShape::Editor_ImGui()
{
	//現在のスフィア一覧
	if (ImGui::TreeNode("Spheres"))
	{
		int cnt = 0;
		for (auto& p : m_spheres)
		{
			ImGui::Text("center : %f,%f,%f", p.center.x, p.center.y, p.center.z);
			ImGui::Text("radius : %f", p.radius);
		}
		ImGui::TreePop();
	}

	//ポイントの追加
	static KdVector3 center = { 0,0,1 };
	static float radius = 1.0f;
	ImGui::DragFloat3(u8"中心点", &center.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"半径", &radius, 0.01f, 0.0f);
	if (ImGui::DragInt(u8"滑らかさ", &m_detail, 4, 30))
	{
		ReCreate();
	}


	if (ImGui::Button(u8"新しいスフィア作成"))
	{
		AddSphere(center, radius);
	}

}

UINT KdPrimitiveRenderer::SphereShape::AddSphere(const KdVector3& center, float radius)
{
	//新しいポイントを登録してよいか
	if (radius <= 0) { return (UINT)m_spheres.size(); }



	//追加する場所を計算
	auto centr = center;
	float rdus = radius;

	SphereInfo sphrInf;

	sphrInf.center = centr;
	sphrInf.radius = rdus;

	m_spheres.push_back(sphrInf);

	//次の描画で頂点の作り直し
	ReCreate();


	return (UINT)m_spheres.size();
}



// 6/2追加
void KdPrimitiveRenderer::Start()
{
	m_polygon = std::make_shared<KdPolygon>();
	m_polygon->Initialize();

	// 軽所の初期化（保存読み込みを実装する時には変更）
	m_shapeType = None;
	m_shape = nullptr;

}
void KdPrimitiveRenderer::PreDraw()
{
	if (m_enable == false) { return; }
	if (m_polygon == nullptr) { return; }
	if (m_shape == nullptr) { return; }
	if (m_shape->IsEstablished() == false) { return; }

	// 何らかの変更があった場合は頂点を作り直す
	m_shape->CreateVertex(m_polygon);

	//描画対象登録
	KdFramework::GetInstance().m_renderingData.
		m_currentScreenData->m_drawList.push_back(this);

}
void KdPrimitiveRenderer::Draw(bool opaque, KdShader::PassTags passTag)
{
	//影用の描画
	if (passTag == KdShader::PassTags::ShadowCaster) { return; }

	auto& sm = KdShaderManager::GetInstance();
	sm.m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();
	sm.m_cbPerDraw->WriteWorkData();	// シェーダーに送り込む

	// 描画
	m_polygon->Draw(KdPolygon::line);
}

void KdPrimitiveRenderer::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	// 形状がまだ確定していない
	if (m_shape == nullptr)
	{
		static ShapeTypes sel = None;	// 選択した形状
		// 形状の選択
		if (ImGui::BeginListBox(u8"作成する形状"))
		{
			if (ImGui::Selectable(u8"Line")) { sel = Line; }
			if (ImGui::Selectable(u8"Sphere")) { sel = Sphere; }
			//これは勝手に生やした
			if (ImGui::Selectable(u8"Box")) { sel = Box; }

			ImGui::EndListBox();
		}

		// 新規作成
		if (sel != None && ImGui::Button(u8"新規作成"))
		{
			//形状作成
			CreateShape(sel);

		}
	}
	// すでに形状が作成された後
	else
	{
		// 表示している形状名
		ImGui::Text(m_shape->GetName().c_str());

		// ラインの色選択
		static KdVector3 color = { 1,1,1 };
		if (ImGui::ColorEdit3(u8"ラインの色", &color.x))
		{
			//0xFF ZZ YY XX

			/*uint32_t r = static_cast<uint8_t> (color.x * 255.0f);
			uint32_t g = static_cast<uint8_t> (color.y * 255.0f);
			uint32_t b = static_cast<uint8_t> (color.y * 255.0f);
			uint32_t a = static_cast<uint8_t> (1 * 255.0f);*/

			UINT r = color.x * 255.0f;
			UINT g = color.y * 255.0f;
			UINT b = color.z * 255.0f;
			UINT a = (1 * 255.0f);

			UINT nc = a << 24 | b << 16 | g << 8 | r;
			m_shape->SetColor(nc);
		}

		//形状ごとのエディタ画面を表示
		m_shape->Editor_ImGui();
	}

	//ImGui::();
}

bool KdPrimitiveRenderer::CreateShape(ShapeTypes type)
{
	std::shared_ptr<ShapeBase> shape = nullptr;

	switch (type)
	{
	case Line:
		shape = std::make_shared<LineShape>();	break;
	case Sphere:
		shape = std::make_shared<SphereShape>(); break;
	case Box:
		break;
	default:
		break;
	}

	// 作成失敗　or まだ未実装の形状を指定したか
	if (shape == nullptr) { return false; }

	SetShape(shape);
	m_shapeType = type;	// IDも覚えておく

	return true;
}

void KdPrimitiveRenderer::SetShape(std::shared_ptr<ShapeBase> shape)
{
	if (shape == nullptr) { return; }

	// 渡された形状を登録
	m_shape = shape;
	m_shape->ReCreate();

}
