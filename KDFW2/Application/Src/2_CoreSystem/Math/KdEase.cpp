
#include "KdMath.h"
#include "KdEase.h"
#include "KdTime.h"

// �C�[�W���O�J�n
void KdEase::Start(Type type, float duration, bool reverse)
{
	m_easeType = type;
	m_durationTime = duration;
	m_reverse = reverse;
	m_startTime = KdTime::GetInstance().GetTime();
}

// �C�[�W���O���̒l�擾
float KdEase::NextValue()
{
	float nowTime = KdTime::GetInstance().GetTime();
	float progressTime = nowTime - m_startTime;

	if (progressTime <= 0)
	{
		if (m_reverse) { return 1.0f; }
		return 0.0f;
	}
	if (m_durationTime <= progressTime) {
		m_isEnd = true;
		if (m_reverse) { return 0.0f; }
		return 1.0f;
	}

	float per = progressTime / m_durationTime;

	float result = 0;
	switch (m_easeType)
	{
		// ���`�⊮
	case Straight:
		result = per;
		break;

		// ������Ƃ�����
	case Sine:
		result = 1 - cos((per * 3.14159f) / 2);
		break;

	case OutSine:
		result = sin((progressTime * KdPI) * 0.5f);
	}

	if (m_reverse) { result = 1 - result; }
	return result;
}
