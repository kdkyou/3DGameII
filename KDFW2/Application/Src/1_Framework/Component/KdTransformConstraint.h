#pragma once

// GameObjectのGUIDを覚えて追従するコンポーネント
// このコンポーネントが付けられたGameObjectのTransformを
// 指定した方法で追従させる

#include"KdComponent.h"

class KdTransformConstraint :public KdComponent
{
public:
	// 追従方法
	enum Type
	{
		Totally,	// 完全追従(授業)
		Position,	// 位置のみ追従
		Rotation,  // 回転のみ追従
		Parent,		// 親子構造のように追従
		LookAt,		// 対象を向き続ける(授業予定)
	};

	virtual void Start()override;
	virtual void Update()override;
	virtual void Editor_ImGui()override;

	virtual void Serialize(nlohmann::json& outJson)const override;
	virtual void Deserialize(const nlohmann::json& jsonObj) override;

	// 追従する対象と追従方法の設定
	void Set(std::shared_ptr<KdGameObject> target,Type chaseType = Totally);

private:

	// 追従するGameObject
	std::weak_ptr<KdGameObject> m_target;
	std::string m_targetGuid = "";	// targetのGUID(絶対に被らない番号)

	Type m_chaseType = LookAt;	// 追従方法



};