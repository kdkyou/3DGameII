#include"KdFramework.h"
#include"Collision.h"

//�t���[�����[�N�ɃR���|�[�l���g�ł��邱�Ƃ�o�^
SetClassAssembly(Collision, "Component");

void Collision::Start()
{

}

void Collision::Update()
{
	//�L�������t���O�`�F�b�N
	if (IsEnable() == false) { return; }

	//�S�Ă̗L��(Enable)�ȃI�u�W�F�N�g���擾����
	auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();

	//Component�������Transform����position�����擾
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetPosition();

	for (auto& ray : m_rayDatas)
	{
		//�L����������
		if (ray.isEnable == false) { continue; }

		//���C����---------------------->
		KdVector3 vStartPos = pos;
		vStartPos += ray.correction;	//�v���C���[�̏��z������i���̍���

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
				bool hit = KdRayToMesh(vStartPos, ray.direction, ray.correction.y, *node.Mesh, oppMat, hitResult);

				if (hit)
				{
					//������������ʒu���獷���߂������������������Z�b�g����
					pos.y += ray.correction.y - hitResult.Distance;
					transform->SetPosition(pos);

				}
			}
		}
	}
	//<----------------------�������̃��C����(���n����)

	for (auto& sphere : m_sphereDatas)
	{
		//�������̓����蔻��(���n����)---------------------->
		// ���̃��b�V���̓����蔻��
		KdVector3 vCenter = pos;	//���̒��S���W
		vCenter += sphere.center;	//���������グ��
		
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

				bool hit = KdSphereToMesh(vCenter, sphere.radius, *node.Mesh, mat, hitResult);

				if (hit&&sphere.isResult)
				{
					//�ړ��������ʂ��猳�̈ʒu������ �����߂�����ݐ�
					sphere.vPushBack += (vMove - vCenter);
				}
			}

		}
		//�㉺�̐����̓��C����ōs���Ă���̂ŕs�v
		sphere.vPushBack.y = 0;
		if (sphere.vPushBack.Length() > 0)
		{
			pos += sphere.vPushBack;
			transform->SetPosition(pos);
		}
	}
	//<----------------------�������̓����蔻��(���n����)

}

void Collision::LateUpdate()
{
}

void Collision::Editor_ImGui()
{
	//�L�������`�F�b�N
	KdComponent::Editor_ImGui();

	// �Ǘ����Ă��郌�C����ꗗ
	if (ImGui::CollapsingHeader(u8"���C����ꗗ", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto&& ray : m_rayDatas)
		{
		ImGui::PushID(&ray);
			if (ImGui::CollapsingHeader(ray.Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				//�����肩�Ǘ��ł���悤�ɖ��O
				ImGui::InputText(u8"�Ǘ���", &ray.Name);
				
				//����̗L������
				ImGui::Checkbox(u8"�L������",&ray.isEnable);

				//���C�̎n�߂�ʒu
				ImGui::DragFloat3(u8"�J�n�n�_", &ray.startPos.x, 0.01f, -1.0f, 1.0f);

				//���C�̕␳�l
				ImGui::DragFloat3(u8"���e�l", &ray.correction.x, 0.01f, 0.0f, 10.0f);
				
				//���C�̕����x�N�g��
				ImGui::DragFloat3(u8"�x�N�g��", &ray.direction.x, 0.01f, -1.0f, 1.0f);
			}
		ImGui::PopID();
		}
	}

	// �Ǘ����Ă���X�t�B�A����ꗗ
	if (ImGui::CollapsingHeader(u8"�X�t�B�A����ꗗ", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto&& sphere : m_sphereDatas)
		{
			ImGui::PushID(&sphere);
			if (ImGui::CollapsingHeader(sphere.Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				//�����肩�Ǘ��ł���悤�ɖ��O
				ImGui::InputText(u8"�Ǘ���", &sphere.Name);

				//����̗L������
				ImGui::Checkbox(u8"�L������", &sphere.isEnable);

				//���茋�ʂ̊i�[���邩���Ȃ���
				ImGui::Checkbox(u8"�x�N�g���i�[", &sphere.isResult);

				//�X�t�B�A�̃Z���^�[�ʒu
				ImGui::DragFloat3(u8"�Z���^�[", &sphere.center.x, 0.01f, -1.0f, 1.0f);

				//�X�t�B�A�̔��a
				ImGui::DragFloat(u8"���a", &sphere.radius, 0.01f, 0.0f, 100.0f);

			}
		ImGui::PopID();
		}
	}

	if(ImGui::Button(u8"���C�ǉ�"))
	{
		AddRay();
	}

	if (ImGui::Button(u8"�X�t�B�A�ǉ�"))
	{
		AddSphere();
	}

}

void Collision::AddRay()
{
	RayColInfo ray;
	ray.Name = "ray";
	m_rayDatas.push_back(ray);
}

void Collision::AddSphere()
{
	SphereColInfo sphere;
	sphere.Name = "sphere";
	m_sphereDatas.push_back(sphere);
}

void Collision::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	//����f�[�^��ۑ�
	auto arr = nlohmann::json::array();
	for (auto&& ray : m_rayDatas)
	{
		nlohmann::json obj;
		obj["Enable"] = ray.isEnable;
		/*obj["RayStartPos"]= ray.startPos;
		obj["RayCorrection"]= ray.correction;
		obj["RayDirection"] = ray.direction;
		*/obj["RayName"] = ray.Name;
		arr.push_back(obj);
	}
	for (auto& sphere : m_sphereDatas)
	{
		nlohmann::json obj;
		obj["Enable"] = sphere.isEnable;
		obj["Result"] = sphere.isResult;
		/*obj["SphereCenter"] = sphere.center;
		*/obj["SphereRadius"] = sphere.radius;
		obj["SphereName"] = sphere.Name;
		arr.push_back(obj);
	}
	outJson["CollisionDatas"] = arr;
	



}

void Collision::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);


}


//void Collision::CollisionPhase()
//{
//	//�S�Ă̗L��(Enable)�ȃI�u�W�F�N�g���擾����
//	auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();
//
//	//Player��Transform����position�����擾
//	const auto& transform = GetGameObject()->GetTransform();
//	auto pos = transform->GetPosition();
//
//	//�������̃��C����(���n����)---------------------->
//	KdVector3 vStartPos = pos;
//	vStartPos.y += m_rayCorrection;	//�v���C���[�̏��z������i���̍���
//
//	//�L�����N�^�[�S���Ɠ����蔻��
//	for (auto&& obj : enableObjects)
//	{
//		//�����蔻��̑Ώۂ��ǂ���
//		if (obj->GetTag() != "CollisionObject") { continue; }
//
//		//�Ώۂ����f���f�[�^�������Ă��邩 GetComponent�̌��Ɍ^���w�肷�� 
//		// #define���Ō^�̐ݒ�����Ă�������ύX�������������₷�����H
//		const auto& modelComp = obj->GetComponent<KdModelRendererComponent>();
//		if (modelComp == nullptr) { continue; }
//
//		//���f���̎������Transform�̎擾
//		const auto& modelTrans = modelComp->GetGameObject()->GetTransform();
//
//		//���f�����̎擾
//		const auto& modelData = modelComp->GetModel();
//		if (modelData == nullptr) { continue; }
//
//		//���f���ɑ΂��ē����蔻��
//		for (auto& node : modelData->GetAllNodes())
//		{
//			//���f���Ɋւ���S�Ă̏��������Ă����
//			//���b�V�����ǂ����𒲂ׂ�
//			if (node.Mesh == nullptr) { continue; }
//
//			//���C����J�n
//			KdRayHit hitResult; //���ʊi�[�ꏊ
//
//			//�e�p�[�c�̌��_����̃Y����
//			const auto& meshMat = modelComp->GetAllNodeTransforms()[node.NodeIndex].GetWorldMatrix();
//			//�e�p�[�c�̌��݂̈ʒu
//			auto oppMat = meshMat * modelTrans->GetWorldMatrix();
//
//			//���b�V���ɑ΂��铖���蔻��
//			bool hit = KdRayToMesh(vStartPos, KdVector3(0, -1, 0), m_rayCorrection + m_fall, *node.Mesh, oppMat, hitResult);
//
//			if (hit)
//			{
//				//������������ʒu���獷���߂������������������Z�b�g����
//				pos.y += m_rayCorrection - hitResult.Distance;
//				transform->SetPosition(pos);
//				m_fall = 0.0f;
//			}
//		}
//	}
//	//<----------------------�������̃��C����(���n����)
//
//
//	//�������̓����蔻��(���n����)---------------------->
//	// ���̃��b�V���̓����蔻��
//	KdVector3 vCenter = pos;	//���̒��S���W
//	vCenter.y += m_sphereCorrection;	//���������グ��
//	KdVector3 vPushBack = {};	//�����߂����x�N�g���̍��v
//
//	//�O�L�����N�^�[�̌J��Ԃ�
//	for (auto&& obj : enableObjects)
//	{
//		//�����蔻��Ώۂ�
//		if (obj->GetTag() != "CollisionObject") { continue; }
//
//		//�Ώۂ̃��f�����
//		const auto& modelComp = obj->GetComponent<KdModelRendererComponent>();
//		if (modelComp == nullptr) { continue; }
//
//		//�Ώۂ̃��f���f�[�^
//		const auto& modelData = modelComp->GetModel();
//		if (modelData == nullptr) { continue; }
//
//		for (auto& node : modelData->GetAllNodes())
//		{
//			if (node.Mesh == nullptr) { continue; }
//
//			auto mat = modelComp->GetAllNodeTransforms()[node.NodeIndex].GetWorldMatrix()
//				* obj->GetTransform()->GetWorldMatrix();
//
//			//������
//			KdSphereHit hitResult;
//			KdVector3 vMove;	//�����߂��ꂽ�ړ���
//
//			hitResult.MovedSpherePos = &vMove;
//
//			bool hit = KdSphereToMesh(vCenter, m_sphereRadius, *node.Mesh, mat, hitResult);
//
//			if (hit)
//			{
//				//�ړ��������ʂ��猳�̈ʒu������ �����߂�����ݐ�
//				vPushBack += (vMove - vCenter);
//			}
//		}
//
//	}
//	//�㉺�̐����̓��C����ōs���Ă���̂ŕs�v
//	vPushBack.y = 0;
//	if (vPushBack.Length() > 0)
//	{
//		pos += vPushBack;
//		transform->SetPosition(pos);
//	}
//
//	//<----------------------�������̓����蔻��(���n����)
//
//}