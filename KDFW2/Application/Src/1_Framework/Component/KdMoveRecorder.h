#pragma once

#include"KdComponent.h"

//#include"KdFramework.h"

class KdMoveRecorder : public KdComponent
{
public:

	virtual  void Start()override;
	virtual void Update()override;
	virtual void Editor_ImGui()override;

	//このループの移動差分を返す
	const KdMatrix& GetDifference() const{ return m_difference; }

	//virtual void Serialize(nlohmann::json& outJson) const override;			// (保存)
	//virtual void Deserialize(const nlohmann::json& jsonObj) override;		// （読み込み）

private:

	KdMatrix m_difference;	// 計算した差分行列
	KdMatrix m_prev;		// 前回の行列



};