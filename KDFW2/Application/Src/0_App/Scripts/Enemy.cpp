#include"KdFramework.h"
#include"Enemy.h"

#include"Collision.h"

//�t���[�����[�N�ɃR���|�[�l���g�ł��邱�Ƃ�o�^
SetClassAssembly(Enemy, "Component");

void Enemy::Start()
{
	m_spCollision = GetGameObject()->
		GetComponent<Collision>();
}

void Enemy::Update()
{
	if (IsEnable() == false) { return; }

	if (m_spCollision == nullptr) { return; }

	std::shared_ptr<KdGameObject> spObj = m_spCollision->GetTarget().lock();
//	auto spObj = m_spCollision->GetTargetb();

	if (spObj == nullptr) { return; }

	//���g�̈ʒu�����擾
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetLocalPosition();

	//�Ώۂ̈ʒu�����擾
	const auto& targetTrans = spObj->GetTransform();
	auto targetPos = targetTrans->GetLocalPosition();

	//�������v�Z
	auto difference = targetPos - pos;

	//���g�̂׃N�g��
	auto nowDir = transform->GetWorldMatrix().Backward();

	//�Ώۈʒu�̃x�N�g��
	auto vec = difference;
	vec.Normalize();


	//�p�x�Z�o
	float nowAng = atan2(nowDir.x, nowDir.z);
	nowAng = DirectX::XMConvertToDegrees(nowAng);

	float targetAng = atan2(vec.x, vec.z);
	targetAng = DirectX::XMConvertToDegrees(targetAng);

	float betweenAng = targetAng - nowAng;
	if (betweenAng > 180)
	{
		betweenAng -= 360;
	}
	else if (betweenAng < -180)
	{
		betweenAng += 360;
	}
	
	float rotateAng = std::clamp(betweenAng, -m_ang, m_ang);

	float deltaTime = KdTime::GetInstance().GetDeltaTime();

	//���݂̉�]���
	KdQuaternion quat = transform->GetLocalRotation();

	KdQuaternion qAdd = KdQuaternion::CreateFromAxisAngle(
		KdVector3(0, 1, 0),					//��]�̒��S��	�O�ς��������낤
		rotateAng * KdDeg2Rad * deltaTime		//��]�p�x(���W�A���p�x)
	);

	//��]��K�p				���Z���������₵�����ꍇ�|���Z
	transform->SetLocalRotation(quat * qAdd);

	//y���W�̍������l�����Ȃ�
	difference.y = 0;
	
	//���������ȏ�̎��ړ������J�n
	if (difference.Length() >= m_difference && rotateAng<1.0f)
	{	
		//���ʕ����̃x�N�g���擾
		KdVector3 vf = transform->GetWorldMatrix().Backward();

		KdVector3 vMoveOnce = vf * m_speed * deltaTime;

		pos += vMoveOnce;

		transform->SetLocalPosition(pos);

	}


}

void Enemy::LateUpdate()
{

}


void Enemy::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["Speed"] = m_speed;
	outJson["Angle"] = m_ang;
	outJson["Difference"] = m_difference;
}

void Enemy::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "Speed", &m_speed);
	KdJsonUtility::GetValue(jsonObj, "Angle", &m_ang);
	KdJsonUtility::GetValue(jsonObj, "Difference", &m_difference);
}

void Enemy::Editor_ImGui()
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

	//��]�p�x
	ImGui::DragFloat(
		u8"�p�x",		//�\����
		&m_ang,		//�֘A�t����ϐ��|�C���^
		0.01f,			//���̍X�V��
		0.0f,			//�ŏ��l
		100.0f			//�ő�l
	);

	//�ێ�����
	ImGui::DragFloat(
		u8"�ێ�����",		//�\����
		&m_difference,		//�֘A�t����ϐ��|�C���^
		0.01f,			//���̍X�V��
		0.0f,			//�ŏ��l
		100.0f			//�ő�l
	);
}
