#include"KdPrimitiveRenderer.h"

//6/2�ǉ�
#include"0_APP/Shader/Polygon/KdPolygon.h"
#include"KdFrameWork.h"

//�`��̃G�f�B�^���
#include"Editor/KdEditorData.h"

//�t���[�����[�N�ɃR���|�[�l���g�ł��邱�Ƃ�o�^
SetClassAssembly(KdPrimitiveRenderer, "Component");


bool KdPrimitiveRenderer::LineShape::CreateVertex(std::shared_ptr<KdPolygon> poly)
{
	//6/2�ǉ�
	if (poly == nullptr) { return false; }

	//���C���ɂȂ��Ă��邩�ǂ���
	if (IsEstablished() == false) { return false; }
	//���łɍŐV�̏�
	if (m_isCreated == true) { return false; }

	//���_�̍쐬-------------------->
	poly->Release();	//�|���S���̍�蒼��

	//��ԍŏ��̓_ = ���_(0,0,0)�X�^�[�g
	poly->AddVertex(KdVector3(0, 0, 0), KdVector2(0, 0), m_color);

	//��_�ڈڍs�͑��݂�����̑S��
	for (auto& p : m_points)
	{
		poly->AddVertex(p, KdVector2(0, 0), m_color);
	}
	//<-------------------->���_�̍쐬

	m_isCreated = true;		///�ŐV�̏��ɂȂ����t���O

	return true;
}

bool KdPrimitiveRenderer::LineShape::IsEstablished()
{
	//���Ƃ��Đ������Ă��Ȃ�
	if (m_points.size() <= 0) { return false; }

	//��true��
	return true;
}

void KdPrimitiveRenderer::LineShape::Editor_ImGui()
{
	//���݂̃|�C���g�ꗗ
	if (ImGui::TreeNode("points"))
	{
		int cnt = 0;
		for (auto& p : m_points)
		{
			ImGui::Text("%f,%f,%f", p.x, p.y, p.z);
		}
		ImGui::TreePop();
	}

	//�|�C���g�̒ǉ�
	static KdVector3 vDir = { 0,0,1 };
	static float length = 1.0f;
	ImGui::DragFloat3(u8"�L�΂�����", &vDir.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"�L�΂�����", &length, 0.01f, 0.0f);

	if (ImGui::Button(u8"�V�����|�C���g�쐬"))
	{
		AddPoint(vDir, length);
	}

}

UINT KdPrimitiveRenderer::LineShape::AddPoint(const KdVector3& dir, float length)
{
	//�V�����|�C���g��o�^���Ă悢��
	if (dir.Length() <= 0.0f) { return (UINT)m_points.size(); }
	if (length <= 0) { return (UINT)m_points.size(); }

	//�J�n�n�_ = �o�^���ꂽ�Ō�̃|�C���g
	KdVector3 vStart = { 0,0,0 };
	if (m_points.size() > 0)
	{
		vStart = *(m_points.end() - 1);		//�o�^���ꂽ�Ō�̗v�f
	}

	//�ǉ�����ꏊ���v�Z
	auto dire = dir;
	dire.Normalize();
	KdVector3 vEnd = vStart + (dire * length);
	m_points.push_back(vEnd);

	//���̕`��Œ��_�̍�蒼��
	ReCreate();


	return (UINT)m_points.size();
}


// ���̕`��
bool KdPrimitiveRenderer::SphereShape::CreateVertex(std::shared_ptr<KdPolygon> poly)
{
	//6/2�ǉ�
	if (poly == nullptr) { return false; }

	//���C���ɂȂ��Ă��邩�ǂ���
	if (IsEstablished() == false) { return false; }
	//���łɍŐV�̏�
	if (m_isCreated == true) { return false; }

	if (m_detail <= 0) { return false; }

	//���_�̍쐬-------------------->
	poly->Release();	//�|���S���̍�蒼��

	KdVector3 pos;

	//pos��center�ł͂Ȃ�pos�ɕς����肷��ƕω�������

	for (auto& s : m_spheres)
	{

		int count =0;

		for (int i = 0; i < m_detail; ++i)
		{

			//�@XZ��
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
			// XY��
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

			// YZ��
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
			// XY��
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
	//<-------------------->���_�̍쐬

	m_isCreated = true;		///�ŐV�̏��ɂȂ����t���O

	return true;
}

bool KdPrimitiveRenderer::SphereShape::IsEstablished()
{
	//���Ƃ��Đ������Ă��Ȃ�
	if (m_spheres.size() <= 0) { return false; }

	//��true��
	return true;
}

void KdPrimitiveRenderer::SphereShape::Editor_ImGui()
{
	//���݂̃X�t�B�A�ꗗ
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

	//�|�C���g�̒ǉ�
	static KdVector3 center = { 0,0,1 };
	static float radius = 1.0f;
	ImGui::DragFloat3(u8"���S�_", &center.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"���a", &radius, 0.01f, 0.0f);
	if (ImGui::DragInt(u8"���炩��", &m_detail, 4, 30))
	{
		ReCreate();
	}


	if (ImGui::Button(u8"�V�����X�t�B�A�쐬"))
	{
		AddSphere(center, radius);
	}

}

UINT KdPrimitiveRenderer::SphereShape::AddSphere(const KdVector3& center, float radius)
{
	//�V�����|�C���g��o�^���Ă悢��
	if (radius <= 0) { return (UINT)m_spheres.size(); }



	//�ǉ�����ꏊ���v�Z
	auto centr = center;
	float rdus = radius;

	SphereInfo sphrInf;

	sphrInf.center = centr;
	sphrInf.radius = rdus;

	m_spheres.push_back(sphrInf);

	//���̕`��Œ��_�̍�蒼��
	ReCreate();


	return (UINT)m_spheres.size();
}



// 6/2�ǉ�
void KdPrimitiveRenderer::Start()
{
	m_polygon = std::make_shared<KdPolygon>();
	m_polygon->Initialize();

	// �y���̏������i�ۑ��ǂݍ��݂��������鎞�ɂ͕ύX�j
	m_shapeType = None;
	m_shape = nullptr;

}
void KdPrimitiveRenderer::PreDraw()
{
	if (m_enable == false) { return; }
	if (m_polygon == nullptr) { return; }
	if (m_shape == nullptr) { return; }
	if (m_shape->IsEstablished() == false) { return; }

	// ���炩�̕ύX���������ꍇ�͒��_����蒼��
	m_shape->CreateVertex(m_polygon);

	//�`��Ώۓo�^
	KdFramework::GetInstance().m_renderingData.
		m_currentScreenData->m_drawList.push_back(this);

}
void KdPrimitiveRenderer::Draw(bool opaque, KdShader::PassTags passTag)
{
	//�e�p�̕`��
	if (passTag == KdShader::PassTags::ShadowCaster) { return; }

	auto& sm = KdShaderManager::GetInstance();
	sm.m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();
	sm.m_cbPerDraw->WriteWorkData();	// �V�F�[�_�[�ɑ��荞��

	// �`��
	m_polygon->Draw(KdPolygon::line);
}

void KdPrimitiveRenderer::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	// �`�󂪂܂��m�肵�Ă��Ȃ�
	if (m_shape == nullptr)
	{
		static ShapeTypes sel = None;	// �I�������`��
		// �`��̑I��
		if (ImGui::BeginListBox(u8"�쐬����`��"))
		{
			if (ImGui::Selectable(u8"Line")) { sel = Line; }
			if (ImGui::Selectable(u8"Sphere")) { sel = Sphere; }
			//����͏���ɐ��₵��
			if (ImGui::Selectable(u8"Box")) { sel = Box; }

			ImGui::EndListBox();
		}

		// �V�K�쐬
		if (sel != None && ImGui::Button(u8"�V�K�쐬"))
		{
			//�`��쐬
			CreateShape(sel);

		}
	}
	// ���łɌ`�󂪍쐬���ꂽ��
	else
	{
		// �\�����Ă���`��
		ImGui::Text(m_shape->GetName().c_str());

		// ���C���̐F�I��
		static KdVector3 color = { 1,1,1 };
		if (ImGui::ColorEdit3(u8"���C���̐F", &color.x))
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

		//�`�󂲂Ƃ̃G�f�B�^��ʂ�\��
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

	// �쐬���s�@or �܂��������̌`����w�肵����
	if (shape == nullptr) { return false; }

	SetShape(shape);
	m_shapeType = type;	// ID���o���Ă���

	return true;
}

void KdPrimitiveRenderer::SetShape(std::shared_ptr<ShapeBase> shape)
{
	if (shape == nullptr) { return; }

	// �n���ꂽ�`���o�^
	m_shape = shape;
	m_shape->ReCreate();

}
