#pragma once

class KdObject : public std::enable_shared_from_this<KdObject>
{
public:

	virtual ~KdObject() {}

	// �폜����
	void Destroy() { m_isDestroy = true; }
	// �폜�t���O�擾
	bool IsDestroy() const { return m_isDestroy; }

	template<class T>
	std::shared_ptr<T> ToSharedPtr(T* thisPtr)
	{
		return std::static_pointer_cast<T>(shared_from_this());
	}

	// �N���X���擾(�����ˑ�)
	std::string GetClassName() const
	{
		// �����ˑ� Visual Studio�̂ݎg�p�\
		return &typeid(*this).name()[6];
	}

private:

	// �폜�t���O
	bool									m_isDestroy = false;

};