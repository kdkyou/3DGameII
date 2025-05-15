#pragma once

//�N���X�̑O���錾
class Gravity;
class ObjectList;

//�����蔻��𐧌䂷��R���|�[�l���g

class Collision :public KdScriptComponent
{
public:

	enum Check
	{
		Ground,
		Player,
		Enemy,
	};


	struct RayColInfo
	{
		std::string Name = "";
		bool isEnable = true;
		bool isGravity = true;
		bool isTarget = false;
		int list = Check::Ground;
		KdVector3 startPos = {0,0,0};
		KdVector3 correction = {0,0,0};
		KdVector3 direction = { 0,0,0 };
	};

	struct SphereColInfo
	{
		std::string Name = "";
		bool isEnable = true;
		bool isResult = true;
		bool isTarget = true;
		int list = Check::Ground;
		KdVector3 center = { 0,0.5f,0 };
		float radius = 0.5f;
		KdVector3 vPushBack = {0,0,0};
	};

	//�����������ǂ����݂̂�Ԃ����C����
	bool RayCast(const KdVector3& startPos, const KdVector3& direction);

	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;

	// ���̃N���X�̓��e��JSON�f�[�^������(�ۑ�)
	virtual void Serialize(nlohmann::json& outJson) const override;

	// JSON�f�[�^����A�N���X�̓��e��ݒ�i�ǂݍ��݁j
	virtual void Deserialize(const nlohmann::json& jsonObj) override;

	//�����蔻��̏��𒲐�����
	virtual void Editor_ImGui()override;

	//�擾�����^�[�Q�b�g����^����
	const std::weak_ptr<KdGameObject>& GetTarget() { return m_wpTarget; }
	const KdGameObject* GetTargetb() { 
		if (m_lock == false)
			return m_wpTargetb;

		else
			return NULL;
	}

private:

	void CollisionRay();

	void AddRay();
	void AddSphere();

	std::vector<RayColInfo> m_rayDatas;
	std::vector<SphereColInfo>m_sphereDatas;

	//�d�̓R���|�[�l���g
	std::shared_ptr<Gravity> m_spGravity = nullptr;
	std::shared_ptr<ObjectList> m_spObjList = nullptr;

	//�^�[�Q�b�g�擾
	std::weak_ptr<KdGameObject> m_wpTarget;
	KdGameObject* m_wpTargetb;
	bool m_lock = true;
	
}; 