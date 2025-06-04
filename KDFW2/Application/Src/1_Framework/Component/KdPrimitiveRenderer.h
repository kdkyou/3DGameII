#pragma once

//��{�}�`�̕`����s��
//�����蔻��͈̔͂⃌�C����̃��C���`��Ɏg��
#include"KdComponent.h"

class KdPolygon;

class KdPrimitiveRenderer : public KdRendererComponent
{
public:
	//�`��ʃf�[�^----------------------------->
		//�\������`��
	enum ShapeTypes
	{
		None,			//�\���`�����܂����܂��ĂȂ�
		Line,			//���C���`��
		Sphere,			//���̕`��(�ۑ�)
		Box,			//�{�b�N�X�`��
	};

	//�e�`��̊��N���X
	class ShapeBase
	{
	public:
		// �`�󖼂̎擾 (�ϐ��ɂ���ꍇ)
		virtual std::string GetName() { return "None"; }

		// �G�f�B�^���
		virtual void Editor_ImGui() {}

		//���_���̍쐬
		virtual bool CreateVertex(std::shared_ptr<KdPolygon> poly) = 0;

		// �`�悵�Ă������̔��f
		virtual bool IsEstablished() { return false; }

		void SetColor(UINT col)
		{
			m_color = col;
			m_isCreated = false;
		}

		void ReCreate() { m_isCreated = false; }

		bool IsCreated() { return m_isCreated; }

	protected:
		// �S�Ă̌`��Ŏg�p����f�[�^
		UINT m_color = 0xFFFFFFFF;
		// ���_�����������
		bool m_isCreated = false;
		// ���_�������ɍ�ꂽ��
		bool m_isCompleted = false;
	};

	//���C���`��
	class LineShape :public ShapeBase
	{
	public:
		std::string GetName() override { return "Line"; }

		bool CreateVertex(std::shared_ptr<KdPolygon> poly)override;

		bool IsEstablished() override;

		void Editor_ImGui()override;

		//�Ō�̒n�_����w������Ɏ��̃|�C���g�����
		//�ŏ��̏ꍇ��GameOBject�̏ꏊ
		UINT AddPoint(const KdVector3& dir, float length);
		UINT AddPoint(const KdVector3& dir, const KdVector3 offset, float length) {}


	private:

		//�o�^���ꂽ�|�C���g�ꗗ
		std::vector<KdVector3> m_points;

	};

	class SphereShape :public ShapeBase
	{
	public:
		std::string GetName() override { return "Sphere"; }

		bool CreateVertex(std::shared_ptr<KdPolygon> poly)override;

		bool IsEstablished() override;

		void Editor_ImGui()override;

		//�Ō�̒n�_����w������Ɏ��̃|�C���g�����
		//�ŏ��̏ꍇ��GameOBject�̏ꏊ
		UINT AddSphere(const KdVector3& center, float radius);

	private:

		//�o�^���ꂽ�|�C���g�ꗗ
		struct SphereInfo
		{
			KdVector3  center = { 0,0,0 };
			float	   radius = 0.0f;
		};

		std::vector<SphereInfo> m_spheres;

		//���̂̊��炩��
		int m_detail = 8;

	};

	//<-----------------------------�`��ʃf�[�^

		// 6/2�ǉ�

	virtual void Start()override;
	virtual void PreDraw()override;
	virtual void Draw(bool opaque, KdShader::PassTags passTag) override;

	virtual void Editor_ImGui();

	//�`��쐬����
	bool CreateShape(ShapeTypes type);
	//�`��ڍׂ����߂ēn���Ă��炤
	void SetShape(std::shared_ptr<ShapeBase> shape);


private:
	// ���ݕ\�����Ă���`��ID
	ShapeTypes		m_shapeType = ShapeTypes::None;
	// �\�����Ă���`��̖{��
	std::shared_ptr<ShapeBase> m_shape = nullptr;
	// �\���Ɏg���|���S��
	std::shared_ptr<KdPolygon> m_polygon = nullptr;
};