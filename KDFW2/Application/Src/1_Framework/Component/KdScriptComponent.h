#pragma once

#include "KdComponent.h"

class KdGameObject;

//=====================================================
//
// スクリプトコンポーネント基底クラス
//
//=====================================================

class KdScriptComponent : public KdComponent
{
public:
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}

	// スクリプトコンポーネントでは不要
	void PreDraw() final {}
	void Draw(int phaseID) final {}

protected:
	

};