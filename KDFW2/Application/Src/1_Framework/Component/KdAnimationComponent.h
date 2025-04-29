#pragma once

#define __DEV_MODE

//=========================================================
// �A�j���[�V�����̊Ǘ��R���|�[�l���g
//=========================================================
#include "KdComponent.h"
class KdModel;

class KdAnimationComponent : public KdComponent
{
public:

	// ���ꂼ��̃m�[�h���v�Z��������
	struct InterpolatedResult
	{
		int nodeIdx = -1;		// �ǂ̃m�[�h�ɑ΂��錋�ʂ�
		float weight = 1.0f;	// ���̃A�j���[�V�����̗��p����

		enum CalcFlag { none = 0, pos = 0x01, rot = 0x02, scale = 0x04 };
		CalcFlag calcFlag = none;	// �ǂ̗v�f�Ɍv�Z���s��ꂽ��

		KdVector3 vPosition;
		KdVector3 vScale;
		KdQuaternion qRotation;

#ifdef __DEV_MODE
		std::string modelNodeName = "";
#endif
	};

	// �A�j���[�V�����f�[�^�ɍĐ����ɕK�v�ȃf�[�^��t������������
	struct RuntimeAnimationData
	{
		std::string		AnimationDataPath = "";			// �ۑ��p�ɓǂݍ��݂Ɏg�����t�@�C���p�X
		std::string		AnimationName = "";				// �Q�[�����ŊǗ��p�̃A�j���[�V������

		// �Đ�����A�j���[�V�����f�[�^
		std::shared_ptr<KdAnimationData> AniamtionData = nullptr;

		std::vector<UINT> ApplyNodeIndex;				// �e�⊮�f�[�^���ǂ̃m�[�h�ɓK�p����邩
#ifdef __DEV_MODE
		std::vector < std::string > ApplyNodeName;
#endif

		double ProgressTime = 0.0;						// ���݂̐i�s����
		float StartTime = 0.0;					// �����̐i�s����

		bool isEnable = false;							// ���̃A�j���[�V�����������Ɏg�����ǂ���
		bool isPlaying = false;							// ���̃A�j���[�V�������X�V���邩�ǂ���
		bool isLoop = false;							// ���[�v���邩�ǂ���

		float speed = 1.0f;								// �Đ��X�s�[�h(1.0�ŃA�j���[�V������������ʂ�)
		float weight = 1.0f;							// ���̃A�j���[�V�����̗��p����(1.0��100%�K�p)

		std::shared_ptr<KdEase> ease = nullptr;			// weight�̕ω��̂�����

		// �A�j���[�V��������X�V����
		// �߂�l�F���̃A�j���[�V�������I������u��
		bool UpdateAnimation(std::shared_ptr<KdModelRendererComponent> modelComp, std::vector<InterpolatedResult>& out);
		bool CreateApplyIndexList(const std::vector<KdModel::Node>& modelNodes);


	};

	KdAnimationComponent();

	virtual void Update() override;
	virtual void Draw(){}

	// �A�j���[�V�����f�[�^�̓ǂݍ���
	// ���󃂃f���f�[�^���璼�œǂݍ��ނ������@���Ȃ�
	void ConvertModelToAnimation();

	// �Ǝ��A�j���[�V�����f�[�^����ǂݍ���
	void LoadFromAnimationDataWithDialog();					// �t�@�C���I���_�C�A���O�\��
	RuntimeAnimationData& LoadFromAnimationData(const std::string& path);
//	RuntimeAnimationData& LoadFromAnimationData(const std::string& path,const float speed,const float startTime);

	// �A�j���[�V�������֘A�t���郂�f��
	void SetModelComponent( std::shared_ptr<KdModelRendererComponent> modelComp );
	
	// �A�j���[�V�����̍Đ�
	void Play(const std::string& name, bool isLoop = false, float crossFadeTime = 0.1f);
	void Play(const std::string& name, float animationSpeed, bool isLoop = false, float crossFadeTime = 0.1f);

	// �A�j���[�V�����Đ��̒�~(�X�V���K�p���I���) �w�肪�Ȃ�������S���~�߂�
	void Stop(const std::string& stopAnimationName = "");

	// �A�j���[�V�����̒��f(�Đ����~�܂邾���œK�p�͂��̂܂�)
	void Suspend(const std::string& suspendAnimationName = "");

	bool IsPlaying(const std::string& animationName = "") const; // ���ݍĐ����̃A�j���[�V�������L�邩�ǂ���

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonObj);
	virtual void Serialize(nlohmann::json& outJson) const;


	// ImGui
	virtual void Editor_ImGui() override;

private:

	// �A�j���[�V���������郂�f���R���|�[�l���g
	std::weak_ptr<KdModelRendererComponent> m_modelComponent;
	std::vector<RuntimeAnimationData> m_runtimeDatas;
	std::vector<InterpolatedResult> m_interpolatedResults; // ���ׂẴA�j���[�V��������񍐂����⊮����
	// �L���Ȃ��ׂẴA�j���[�V������������������
	std::shared_ptr< std::vector<KdTransform> > m_calculatedTransform = nullptr;
};