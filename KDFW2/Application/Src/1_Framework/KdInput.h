#pragma once

#include "CoreSystem.h"
#include "GamePad.h"

//===========================================
// 
// 入力
// 
//===========================================
class KdInput
{
public:

	// マウス
	const DirectX::Mouse::State& GetMouseState() const { return m_mouseState; }
	DirectX::Mouse& GetMouse() { return m_mouse; }
	DirectX::Mouse::ButtonStateTracker& GetMouseTracker() { return m_mouseTracker; }

	// キーボード
	const DirectX::Keyboard::State& GetKeyboardState() const { return m_keyboardState; }
	DirectX::Keyboard& GetKeyboard() { return m_keyboard; }
	DirectX::Keyboard::KeyboardStateTracker& GetKeyboardTracker() { return m_keyboardTracker; }

	//保存された入力データ数の変更
	void SetLength(int length) { m_MaxDataLength = length; }

	void SetWindow(HWND hWnd);

	void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

	void Update();

private:

	DirectX::Mouse::State				m_mouseState = {};
	DirectX::Mouse						m_mouse = {};
	DirectX::Mouse::ButtonStateTracker	m_mouseTracker = {};

	DirectX::Keyboard::State				m_keyboardState = {};
	DirectX::Keyboard						m_keyboard = {};
	DirectX::Keyboard::KeyboardStateTracker m_keyboardTracker = {};

	DirectX::GamePad::State					m_gamepadState = {};
	DirectX::GamePad						m_gamepad = {};
	//DirectX::GamePad::GamePadStateTracker  m_gamepadTracker = {};

	

	std::vector<DirectX::Keyboard::State>	m_keyDatas;
	std::vector<DirectX::Mouse::State>	m_mouseDatas;
	int										m_MaxDataLength = 10;

//-------------------------------
// シングルトン
//-------------------------------
private:
	KdInput()
	{
	}
public:
	static KdInput& GetInstance() {
		static KdInput instance;
		return instance;
	}

};