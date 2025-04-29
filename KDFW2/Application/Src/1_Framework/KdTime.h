#pragma once

//===========================================
// 
// ゲーム時間
// 
//===========================================
class KdTime
{
public:

	float GetTime() { return m_time; }
	float GetDeltaTime() { return m_deltaTime; }

	void Initialize();

	void Update();


private:
	std::chrono::system_clock::time_point m_startTime;

	float m_time;
	float m_deltaTime;

	//-------------------------------
	// シングルトン
	//-------------------------------
private:
	KdTime()
	{
	}
public:
	static KdTime& GetInstance() {
		static KdTime instance;
		return instance;
	}
};