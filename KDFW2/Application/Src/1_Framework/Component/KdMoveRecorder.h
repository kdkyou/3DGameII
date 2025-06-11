#pragma once

#include"KdComponent.h"

//#include"KdFramework.h"

class KdMoveRecorder : public KdComponent
{
public:

	virtual  void Start()override;
	virtual void Update()override;
	virtual void Editor_ImGui()override;

	//���̃��[�v�̈ړ�������Ԃ�
	const KdMatrix& GetDifference() const{ return m_difference; }

	//virtual void Serialize(nlohmann::json& outJson) const override;			// (�ۑ�)
	//virtual void Deserialize(const nlohmann::json& jsonObj) override;		// �i�ǂݍ��݁j

private:

	KdMatrix m_difference;	// �v�Z���������s��
	KdMatrix m_prev;		// �O��̍s��



};