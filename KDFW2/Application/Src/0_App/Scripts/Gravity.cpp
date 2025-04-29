#include"KdFramework.h"
#include"Gravity.h"

void Gravity::Start()
{

}

void Gravity::Update()
{
	if (IsEnable() == false) { return; }

	//���͌��m
	auto& keyboard = KdInput::GetInstance().GetKeyboardState();

	//�P�b�Ԃ̗ʁ�1�t���[���̗�
	float deltaTime = KdTime::GetInstance().GetDeltaTime();

	//�R���|�[�l���g�̎�����̃g�����X�t�H�[���ƍ��W�̎擾
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetPosition();

//	float vMoveOnes = 0.0f;
	KdVector3 vMoveOnes = { 0,0,0 };

	bool isJump = false;

	//radiobutton�̃f�[�^�ɂ���������
	if (m_isSelect==Select::Key) {
		if (keyboard.Space) {
			isJump = true;
		}
	}
	else {
		//�C���^�[�o������
		--m_interval;

		if (m_interval < Select::None)
		{
			isJump = true;
			m_interval = m_intervalVal;
		}
	}

	if (isJump)
	{
		//�����l���W�����v�͂ɕύX
		m_fall = -m_jumpPow;
	}
	
	vMoveOnes.y -= m_fall;
	m_fall += m_gravity * deltaTime;

	pos += vMoveOnes;

	//�X�V���ʂ�Transform�ɋ�����
	transform->SetLocalPosition(pos);
}

void Gravity::LateUpdate()
{
}

void Gravity::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//�L�[���͂�������
	ImGui::Text(u8"�W�����v�^�C�~���O");
	ImGui::RadioButton(u8"�Ȃ�", &m_isSelect,Select::None);
	ImGui::RadioButton(u8"�X�y�[�X", &m_isSelect,Select::Key);
	ImGui::RadioButton(u8"����", &m_isSelect,Select::Auto);

	//�W�����v�͒���
	ImGui::DragFloat(u8"�W�����v��",&m_jumpPow,0.001f,0.0f,100.0f	);

	//�d�͒���
	ImGui::DragFloat(
		u8"�d��",
		&m_gravity,
		0.001f,
		0.0f,
		100.0f
	);

	//�������̃C���^�[�o��
	ImGui::DragFloat(u8"�C���^�[�o���l", &m_intervalVal, 0.001f, 0.0f, 100.0f);

	ImGui::LabelText(std::to_string(m_interval).c_str(), u8"���ݎ���");
}

void Gravity::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);
}

void Gravity::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);
}
