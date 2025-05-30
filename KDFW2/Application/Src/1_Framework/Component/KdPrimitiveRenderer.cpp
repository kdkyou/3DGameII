#include"KdPrimitiveRenderer.h"

bool KdPrimitiveRenderer::LineShape::CreateVertex(std::shared_ptr<KdPolygon> poly)
{
	if (IsEstablished() == false) { return false; }
	return false;
}

bool KdPrimitiveRenderer::LineShape::IsEstablished()
{
	//���Ƃ��Đ������Ă��Ȃ�
	if (m_points.size() <= 0) { return false; }


	return true;
}

//���C���`��̃G�f�B�^���
#include"Editor/KdEditorData.h"
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
	ImGui::DragFloat3(u8"�L�΂�����", &vDir.x, 0.01f,0.0f,1.0f);
	ImGui::DragFloat3(u8"�L�΂�����", &length, 0.01f,0.0f);

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
	KdVector3 vStart = {0,0,0};
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


	return  m_points.size();
}

