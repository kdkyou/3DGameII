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

	//�L�����N�^�[�̑O����
	KdVector3 vf = transform->GetWorldMatrix().Backward();
	KdVector3 vr = transform->GetWorldMatrix().Right();
	KdVector3 vl = transform->GetWorldMatrix().Left();
	KdVector3 vb = transform->GetWorldMatrix().Forward();

	vf.Normalize();
	vr.Normalize();
	vl.Normalize();
	vb.Normalize();

	m_vMoveOnce = KdVector3::Zero;

	if (keyboard.W)
	{
		m_vMoveOnce = vf * m_speed * deltaTime;
	}
	/*if (keyboard.W)
	{
		m_vMoveOnce.z = 1.0f;
	}*/
	if (keyboard.A)
	{
		m_vMoveOnce = vl * m_speed * deltaTime;
	}
	if (keyboard.S)
	{
		m_vMoveOnce = vb * m_speed * deltaTime;
	}
	if (keyboard.D)
	{
		m_vMoveOnce = vr * m_speed * deltaTime;
	}

	//���K��
	m_vMoveOnce.Normalize();
	m_vMoveOnce = m_vMoveOnce * m_speed * deltaTime;

	// ��]
	//���݂̉�]���
	KdQuaternion quat = transform->GetLocalRotation();

	//���Z��������]
	KdQuaternion qAdd = KdQuaternion::CreateFromAxisAngle(
		KdVector3(0,1,0),					//��]�̒��S��	�O�ς��������낤
		20.0f*KdDeg2Rad * deltaTime		//��]�p�x(���W�A���p�x)
	);

	//��]��K�p				���Z���������₵�����ꍇ�|���Z
	transform->SetLocalRotation(quat * qAdd);


	////�W�����v
	//if (keyboard.Space)
	//{
	//	m_fall = -m_jumpPow;
	//}

	////����
	//m_vMoveOnce.y -= m_fall;
	//m_fall += m_gravity * deltaTime;

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

			//�G�t�F�N�g����!!
			auto effect = KdFramework::GetInstance().GetScene()->Instantiate(
				"./Assets/Prefabs/Slash.kdprefab",
				nullptr		//�G�t�F�N�g�͒u���Ă���
			);
			auto pos = transform->GetLocalPosition();
			pos.y += 1.0f;
			pos.x += 0.3f;

			//�G�t�F�N�g���v���C���[�̈ʒu�Ɉړ�
			effect->GetTransform()->SetLocalPosition(pos);
		}
	}


	//�����蔻��
//	CollisionPhase();

}

void Player::LateUpdate()
{

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

}

//Player�ŕK�v�ȃf�[�^�̕ۑ�
void Player::Serialize(nlohmann::json& outJson) const
{
	//�ۑ����K�v�ȃf�[�^��[������]-[�f�[�^]�̃Z�b�g�Ŋo����

	// ���N���X�̊֐������s���Ă���
	KdComponent::Serialize(outJson);

	outJson["Speed"] = m_speed;

}

void Player::Deserialize(const nlohmann::json& jsonObj)
{
	//�ۑ����ꂽ�f�[�^���t�@�C������Ăяo��
	//�f�[�^�Ɋi�[������
	//�p�����̊֐������s
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "Speed", &m_speed);
}

