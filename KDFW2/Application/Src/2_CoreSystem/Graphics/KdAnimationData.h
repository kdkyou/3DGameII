#pragma once



//==========================================================
// ��̃A�j���[�V�����̃f�[�^
//
//==========================================================
struct aiAnimation;
struct PerNode;

struct KdAnimationData : public KdObject
{
public:
	// Assimp����ǂݍ���ŗ����f�[�^��������̃t�H�[�}�b�g�ɕϊ�
	void Parse(aiAnimation* src, const std::vector<KdMeshScene::NodeData>& allNodes);

	// �ۑ�
	bool Save(const std::string& directory)const;
	nlohmann::json Serialize() const;

	// �ǂݍ���
	bool Load(const std::string& filepath);
	bool Deserialize(const nlohmann::json& jsonObj);

	// �A�j���[�V�����f�[�^�̃t�@�C���p�X
	std::string FilePath = "";

	// ���̃A�j���[�V�����S�̂̏��
	std::string Name = "";
	double LengthInTick = 0.0f;		// Tick(�����t���[���Ɠ��`)�P�ʂł̒���
	double TicksParSecond = 0.0f;	// ��b�Ԃ̐i�sTick��(0�̉\������)
	double LengthInSecond = 0.0f;	// �A�j���[�V�����̌p�����ԁi�b�P�ʁj

	// 1�p�[�c(�{�[��)���Ƃ̏��
	struct PerNode
	{
		std::string Name = "";		// �Ώۃm�[�h�̖��O
		int NodeIndex = -1;			// �Ή�����m�[�h��Index���R���|�[�l���g�ŊǗ�

		// �t���[��(�p��)�P�ʂł̈ړ��E��]�E�g�k�̃f�[�^
		struct PositionKey
		{
			double Time = 0;
			KdVector3 vPos;
			KdVector3 Interpolation(const PositionKey& last, double time) const;
		};
		struct RotationKey
		{
			double Time = 0;
			KdQuaternion qRot;

			// �Ăяo�����L�[��擪�A�������Ō�Ƃ��ē�_�Ԃ̃L�[�̕⊮
			KdQuaternion Interpolation(const RotationKey& last, double time)const;
		};
		struct ScaleKey
		{
			double Time = 0;
			KdVector3 vScale;
			KdVector3 Interpolation(const ScaleKey& last, double time)const;
		};

		// Index�w��ł̃L�[�擾
		const PositionKey&	GetPositionKey(int idx) const;
		const RotationKey&	GetRotationKey(int idx) const;
		const ScaleKey&		GetScaleKey(int idx) const;

		// �t���[���w��ł̑O��̃L�[�擾
		std::tuple<const PositionKey&, const PositionKey&>	GetPositionKeyByTime(double frame) const;
		std::tuple<const RotationKey&, const RotationKey&>	GetRotationKeyByTime(double frame) const;
		std::tuple<const ScaleKey&, const ScaleKey&>		GetScaleKeyByTime(double frame) const;

		// �t���[���w��ł̗v�f�̕⊮
		KdVector3		InterpolationPosition(double time) const;
		KdQuaternion	InterpolationRotation(double time) const;
		KdVector3		InterpolationScale(double time) const;

		// Save Load
		nlohmann::json Serialize()const;
		bool Deserialize(const nlohmann::json& jsonObj);

		// �e�f�[�^
		std::vector<PositionKey> PositionKeys;
		std::vector<RotationKey> RotationKeys;
		std::vector<ScaleKey> ScaleKeys;
	};
	std::vector<PerNode> PerNodes;

	// �w�肳�ꂽ�m�[�h�̕⊮���s���c�O���錾�ŉ����ł��Ȃ��̂͂Ȃ�łȂ񂾂�
	KdTransform Interpolation(const PerNode& node, double time);
};