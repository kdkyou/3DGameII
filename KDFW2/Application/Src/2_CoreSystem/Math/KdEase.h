#pragma once


// イージング用管理クラス
class KdEase
{
public:
	// 変化方法
	enum Type { Straight, Sine };

	KdEase() {}
	KdEase(Type t, float d) { Start(t, d); }

	void Start(Type type, float duration = 1.0f, bool reverse = false);
	float NextValue();
	bool IsEnd() const { return m_isEnd; }
private:
	Type m_easeType = Straight;
	float m_startTime = -1;
	float m_durationTime = -1;
	bool m_reverse = false;
	bool m_isEnd = false;
};
