#pragma once

//基本図形の描画を行う
//当たり判定の範囲やレイ判定のライン描画に使う
#include"KdComponent.h"

class KdPolygon;

class KdPrimitiveRenderer: public KdRendererComponent
{
public:
	//表示する形状
	enum ShapeTypes
	{
		None,			//表示形式がまだ決まってない
		Line,			//ライン描画
		Sphere,			//球体描画(課題)
		Box,			//ボックス描画
	};
	
	//各形状の基底クラス
	class ShapeBase 
	{
	public:
		// 形状名の取得 (変数にする場合)
		virtual std::string GetName() { return "None"; }
		
		// エディタ画面
		virtual void Editor_ImGui(){}

		//頂点情報の作成
		virtual bool CreateVertex(std::shared_ptr<KdPolygon> poly) = 0;

		// 描画していいかの判断
		virtual bool IsEstablished() { return false; }

		void SetColor(UINT col)
		{
			m_color = col;
			m_isCreated = false;
		}

		void ReCreate() { m_isCreated = false; }

		bool IsCreated() { return m_isCreated; }

	protected:
		// 全ての形状で使用するデータ
		UINT m_color = 0xFFFFFFFF;
		// 頂点情報を作ったか
		bool m_isCreated = false;
		// 頂点が無事に作れたか
		bool m_isCompleted = false;
	};

	//ライン描画
	class LineShape :public ShapeBase
	{
	public:
		std::string GetName() override { return "Line"; }

		bool CreateVertex(std::shared_ptr<KdPolygon> poly)override;

		bool IsEstablished() override;

		void Editor_ImGui()override;

		//最後の地点から指定方向に次のポイントを作る
		//最初の場合はGameOBjectの場所
		UINT AddPoint(const KdVector3& dir, float length);
		UINT AddPoint(const KdVector3& dir,const KdVector3 offset, float length){}


	private:

		//登録されたポイント一覧
		std::vector<KdVector3> m_points;
	};


	ShapeTypes		m_type = ShapeTypes::None;

};