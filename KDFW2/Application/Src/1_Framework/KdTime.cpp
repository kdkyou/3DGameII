#include "KdTime.h"

void KdTime::Initialize()
{
	m_startTime = std::chrono::system_clock::now();
}

void KdTime::Update()
{
	auto endTime = std::chrono::system_clock::now();
	auto durationTime = endTime - m_startTime;
	auto msec = std::chrono::duration_cast<std::chrono::microseconds>(durationTime).count();

	float prevTime = m_time;

	m_time = msec / 1000000.0f;

	m_deltaTime = m_time - prevTime;

	std::this_thread::sleep_for(std::chrono::milliseconds(16));

}
