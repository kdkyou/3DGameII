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
		}

		//���p���`(�A�j���[�V�����x��)
		if (keyboard.L)
		{
			m_animation->Play("Attack", 11.0f);
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

