#pragma once

#include "CoreSystem.h"

//===========================================
// 
// ����
// 
//===========================================
class KdInput
{
public:

	// �}�E�X
	const DirectX::Mouse::State& GetMouseState() const { return m_mouseState; }
	DirectX::Mouse& GetMouse() { return m_mouse; }
	DirectX::Mouse::ButtonStateTracker& GetMouseTracker() { return m_mouseTracker; }

	// �L�[�{�[�h
	const DirectX::Keyboard::State& GetKeyboardState() const { return m_keyboardState; }
	DirectX::Keyboard& GetKeyboard() { return m_keyboard; }
	DirectX::Keyboard::KeyboardStateTracker& GetKeyboardTracker() { return m_keyboardTracker; }

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

//-------------------------------
// �V���O���g��
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