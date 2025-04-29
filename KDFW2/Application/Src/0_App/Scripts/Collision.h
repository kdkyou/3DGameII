#pragma once

//�����蔻��𐧌䂷��R���|�[�l���g

class Collision :public KdScriptComponent
{
public:

	struct RayColInfo
	{
		std::string Name = "";
		bool isEnable = true;
		KdVector3 startPos = {0,0,0};
		KdVector3 correction = {0,0,0};
		KdVector3 direction = { 0,0,0 };
	};

	struct SphereColInfo
	{
		std::string Name = "";
		bool isEnable = true;
		bool isResult = true;
		KdVector3 center = { 0,0.5f,0 };
		float radius = 0.5f;
		KdVector3 vPushBack = {0,0,0};
	};


	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;

	// ���̃N���X�̓��e��JSON�f�[�^������(�ۑ�)
	virtual void Serialize(nlohmann::json& outJson) const override;

	// JSON�f�[�^����A�N���X�̓��e��ݒ�i�ǂݍ��݁j
	virtual void Deserialize(const nlohmann::json& jsonObj) override;

	//�����蔻��̏��𒲐�����
	virtual void Editor_ImGui()override;

private:

	void AddRay();
	void AddSphere();

	std::vector<RayColInfo> m_rayDatas;
	std::vector<SphereColInfo>m_sphereDatas;
	
}; 