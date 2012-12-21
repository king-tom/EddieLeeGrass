#pragma once

#define WIN32_LEAN_AND_MEAN


#include <windows.h>


#include "CApp.h"


class CSystem
{
public:
	CSystem();
	CSystem(const CSystem&);
	~CSystem();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	char * m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	CApp* m_Application;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


static CSystem* ApplicationHandle = 0;