#include"KdFramework.h"
#include"Player.h"

//�t���[�����[�N�ɃR���|�[�l���g�ł��邱�Ƃ�o�^
SetClassAssembly(Player, "Component");

void Player::Start()
{
	m_animation = GetGameObject()->
		GetComponent<KdAnimationComponent>();
}

void Player::Update()
{
	//�L�������t���O�`�F�b�N
	if (IsEnable() == false) { return; }



	//Player��Transfoem���擾
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetLocalPosition();

	//���͌��m
	auto& keyboard = KdInput::GetInstance().GetKeyboardState();
	auto& mouse = KdInput::GetInstance().GetMouseState();

	//float speed = 0.01f;	//1�t���[���ɓ����������ړ��� ����
	//float speed = 1.0f;	//�P�b�Ԃɓ�����������

	//�P�b�Ԃ̗ʁ�1�t���[���̗�
	float deltaTime = KdTime::GetInstance().GetDeltaTime();

	//�Q�̐��l�̏�Z�ŗʂ����߂�

	//�L�[�{�[�h�̓����ɂ���đO�㍶�E�̈ړ�
	//�������ɗ������鏈��&�W�����v�̏���
	//���̏��A���ʂ�y=0�̈ʒu�Ƃ���

	m_vMoveOnce = KdVector3::Zero;

	if (keyboard.W)
	{
		m_vMoveOnce.z = 1.0f;
	}
	if (keyboard.A)
	{
		m_vMoveOnce.x = -1.0f;
	}
	if (keyboard.S)
	{
		m_vMoveOnce.z = -1.0f;
	}
	if (keyboard.D)
	{
		m_vMoveOnce.x = 1.0f;
	}

	//���K��
	m_vMoveOnce.Normalize();
	m_vMoveOnce = m_vMoveOnce * m_speed * deltaTime;

	//�W�����v
	if (keyboard.Space)
	{
		m_fall = -m_jumpPow;
	}

	//����
	m_vMoveOnce.y -= m_fall;
	m_fall += m_gravity * deltaTime;


	pos += m_vMoveOnce;

	//���n�ʔ���
	/*if (pos.y <- 0.5f)
	{
		pos.y =- 0.5f;
		m_fall = 0.0f;
	}*/

	//�X�V���ʂ�Transform�ɋ�����
	transform->SetLocalPosition(pos);

	//�A�j���[�V���������݂��Ă��邩�ǂ���
	if (m_animation == nullptr) { return; }

	//�U��		�Q�b�g�R���|�[�l���g�͏d�������Ȃ̂�Update�ł��Ȃ�
	if (m_animation->IsPlaying("Attack") == false)
	{
		//��p���`(�A�j���[�V��������)
		if (keyboard.K)
		{
			m_animation->Play("Attack", 22.0f);
		}

		//���p���`(�A�j���[�V�����x��)
		if (keyboard.L)
		{
			m_animation->Play("Attack", 11.0f);
		}

	}


	//�����蔻��
	CollisionPhase();

}

void Player::LateUpdate()
{

}

void Player::CollisionPhase()
{
	//�S�Ă̗L��(Enable)�ȃI�u�W�F�N�g���擾����
	auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();

	//Player��Transform����position�����擾
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetPosition();

	//�������̃��C����(���n����)---------------------->
	float fallDist = -m_vMoveOnce.y;//	���̗�����
	if (fallDist < 0) {
		fallDist = 0;
	}

	KdVector3 vStartPos = pos;
	vStartPos.y += m_rayCorrection	//�v���C���[�̏��z������i���̍���
		+ fallDist;			//���̗����ʂ����Z���Ă���

	//�L�����N�^�[�S���Ɠ����蔻��
	for (auto&& obj : enableObjects)
	{
		//�����蔻��̑Ώۂ��ǂ���
		if (obj->GetTag() != "CollisionObject") { continue; }

		//�Ώۂ����f���f�[�^�������Ă��邩 GetComponent�̌��Ɍ^���w�肷�� 
		// #define���Ō^�̐ݒ�����Ă�������ύX�������������₷�����H
		const auto& modelComp = obj->GetComponent<KdModelRendererComponent>();
		if (modelComp == nullptr) { continue; }

		//���f���̎������Transform�̎擾
		const auto& modelTrans = modelComp->GetGameObject()->GetTransform();

		//���f�����̎擾
		const auto& modelData = modelComp->GetModel();
		if (modelData == nullptr) { continue; }

		//���f���ɑ΂��ē����蔻��
		for (auto& node : modelData->GetAllNodes())
		{
			//���f���Ɋւ���S�Ă̏��������Ă����
			//���b�V�����ǂ����𒲂ׂ�
			if (node.Mesh == nullptr) { continue; }

			//���C����J�n
			KdRayHit hitResult; //���ʊi�[�ꏊ

			//�e�p�[�c�̌��_����̃Y����
			const auto& meshMat = modelComp->GetAllNodeTransforms()[node.NodeIndex].GetWorldMatrix();
			//�e�p�[�c�̌��݂̈ʒu
			auto oppMat = meshMat * modelTrans->GetWorldMatrix();

			//���b�V���ɑ΂��铖���蔻��
			bool hit = KdRayToMesh(vStartPos, KdVector3(0, -1, 0), m_rayCorrection + fallDist, *node.Mesh, oppMat, hitResult);

			if (hit)
			{
				//������������ʒu����͂ݏo�����ō����߂��������Z�b�g����
				pos.y += (m_rayCorrection + fallDist) - hitResult.Distance;
				transform->SetPosition(pos);
				m_fall = 0.0f;
			}
		}
	}
	//<----------------------�������̃��C����(���n����)


	//�������̓����蔻��(���n����)---------------------->
	// ���̃��b�V���̓����蔻��
	KdVector3 vCenter = pos;	//���̒��S���W
	vCenter.y += m_sphereCorrection;	//���������グ��
	KdVector3 vPushBack = {};	//�����߂����x�N�g���̍��v

	//�O�L�����N�^�[�̌J��Ԃ�
	for (auto&& obj : enableObjects)
	{
		//�����蔻��Ώۂ�
		if (obj->GetTag() != "CollisionObject") { continue; }

		//�Ώۂ̃��f�����
		const auto& modelComp = obj->GetComponent<KdModelRendererComponent>();
		if (modelComp == nullptr) { continue; }

		//�Ώۂ̃��f���f�[�^
		const auto& modelData = modelComp->GetModel();
		if (modelData == nullptr) { continue; }

		for (auto& node : modelData->GetAllNodes())
		{
			if (node.Mesh == nullptr) { continue; }

			auto mat = modelComp->GetAllNodeTransforms()[node.NodeIndex].GetWorldMatrix()
				* obj->GetTransform()->GetWorldMatrix();

			//������
			KdSphereHit hitResult;
			KdVector3 vMove;	//�����߂��ꂽ�ړ���

			hitResult.MovedSpherePos = &vMove;

			bool hit = KdSphereToMesh(vCenter, m_sphereRadius, *node.Mesh, mat, hitResult);

			if (hit)
			{
				//�ړ��������ʂ��猳�̈ʒu������ �����߂�����ݐ�
				vPushBack += (vMove - vCenter);
			}
		}

	}
	//�㉺�̐����̓��C����ōs���Ă���̂ŕs�v
	vPushBack.y = 0;
	if (vPushBack.Length() > 0)
	{
		pos += vPushBack;
		transform->SetPosition(pos);
	}

	//<----------------------�������̓����蔻��(���n����)

}

void Player::Editor_ImGui()
{
	//���N���X�̊֐����Ăяo��
	//�L�������`�F�b�N
	KdComponent::Editor_ImGui();

	//�ړ��X�s�[�h�̒���
	ImGui::DragFloat(
		u8"���x",		//�\����
		&m_speed,		//�֘A�t����ϐ��|�C���^
		0.01f,			//���̍X�V��
		0.0f,			//�ŏ��l
		100.0f			//�ő�l
	);

	//�W�����v�͒���
	ImGui::DragFloat(
		u8"�W�����v��",
		&m_jumpPow,
		0.01f,
		0.0f,
		100.0f
	);

	//�d�͒���
	ImGui::DragFloat(
		u8"�d��",
		&m_gravity,
		0.001f,
		0.0f,
		10.0f
	);

	//�����蔻��̕␳�l
	ImGui::DragFloat(
		u8"���C�␳�l",
		&m_rayCorrection,
		0.001f,
		0.0f,
		10.0f
	);

	ImGui::DragFloat(
		u8"�X�t�B�A�␳�l",
		&m_sphereCorrection,
		0.001f,
		0.0f,
		1.0f
	);

	ImGui::DragFloat(
		u8"�X�t�B�A���a",
		&m_sphereRadius,
		0.01f,
		0.0f,
		100.0f
	);

}

//Player�ŕK�v�ȃf�[�^�̕ۑ�
void Player::Serialize(nlohmann::json& outJson) const
{
	//�ۑ����K�v�ȃf�[�^��[������]-[�f�[�^]�̃Z�b�g�Ŋo����

	// ���N���X�̊֐������s���Ă���
	KdComponent::Serialize(outJson);

	outJson["Speed"] = m_speed;
	outJson["Gravity"] = m_gravity;
	outJson["JumpPow"] = m_jumpPow;
	/*outJson["RayCorrection"] = m_rayCorrection;
	outJson["SphereCorrection"] = m_sphereCorrection;
	outJson["SphereRadius"] = m_sphereRadius;*/

	//outJson["Speed"]=KdJsonUtility::CreateArray(&m_speed, 1);
}

void Player::Deserialize(const nlohmann::json& jsonObj)
{
	//�ۑ����ꂽ�f�[�^���t�@�C������Ăяo��
	//�f�[�^�Ɋi�[������
	//�p�����̊֐������s
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "Speed", &m_speed);
	KdJsonUtility::GetValue(jsonObj, "Gravity", &m_gravity);
	KdJsonUtility::GetValue(jsonObj, "JumpPow", &m_jumpPow);
	/*KdJsonUtility::GetValue(jsonObj, "RayCorrection", &m_rayCorrection);
	KdJsonUtility::GetValue(jsonObj, "SphereCorrection", &m_sphereCorrection);
	KdJsonUtility::GetValue(jsonObj, "SphereRadius", &m_sphereRadius);*/
}

