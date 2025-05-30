#include"KdPrimitiveRenderer.h"

bool KdPrimitiveRenderer::LineShape::CreateVertex(std::shared_ptr<KdPolygon> poly)
{
	if (IsEstablished() == false) { return false; }
	return false;
}

bool KdPrimitiveRenderer::LineShape::IsEstablished()
{
	//線として成立していない
	if (m_points.size() <= 0) { return false; }


	return true;
}

//ライン描画のエディタ画面
#include"Editor/KdEditorData.h"
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
	ImGui::DragFloat3(u8"伸ばす方向", &vDir.x, 0.01f,0.0f,1.0f);
	ImGui::DragFloat3(u8"伸ばす距離", &length, 0.01f,0.0f);

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
	KdVector3 vStart = {0,0,0};
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


	return  m_points.size();
}

