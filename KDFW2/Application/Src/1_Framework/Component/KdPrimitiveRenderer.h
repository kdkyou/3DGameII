#pragma once

//��{�}�`�̕`����s��
//�����蔻��͈̔͂⃌�C����̃��C���`��Ɏg��
#include"KdComponent.h"

class KdPolygon;

class KdPrimitiveRenderer: public KdRendererComponent
{
public:
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
		virtual void Editor_ImGui(){}

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
		UINT AddPoint(const KdVector3& dir,const KdVector3 offset, float length){}


	private:

		//�o�^���ꂽ�|�C���g�ꗗ
		std::vector<KdVector3> m_points;
	};


	ShapeTypes		m_type = ShapeTypes::None;

};