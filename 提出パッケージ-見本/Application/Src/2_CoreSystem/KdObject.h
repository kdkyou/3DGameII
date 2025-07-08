#pragma once

class KdObject : public std::enable_shared_from_this<KdObject>
{
public:

	virtual ~KdObject() {}

	// 削除する
	void Destroy() { m_isDestroy = true; }
	// 削除フラグ取得
	bool IsDestroy() const { return m_isDestroy; }

	template<class T>
	std::shared_ptr<T> ToSharedPtr(T* thisPtr)
	{
		return std::static_pointer_cast<T>(shared_from_this());
	}

	// クラス名取得(※環境依存)
	std::string GetClassName() const
	{
		// ※環境依存 Visual Studioのみ使用可能
		return &typeid(*this).name()[6];
	}

private:

	// 削除フラグ
	bool									m_isDestroy = false;

};