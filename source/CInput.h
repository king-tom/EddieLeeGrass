#pragma once

#define DIRECTINPUT_VERSION 0x0800


#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


#include <dinput.h>


class CClass
{
public:
	CClass();
	CClass(const CClass&);
	~CClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void GetMouseLocation(int&, int&);

	bool IsEscapePressed();
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();
	bool IsAPressed();
	bool IsZPressed();
	bool IsXPressed();
	bool IsPgUpPressed();
	bool IsPgDownPressed();
	bool IsLShiftPressed();
	bool IsOnePressed();
	bool IsRPressed();
	bool IsFPressed();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};
