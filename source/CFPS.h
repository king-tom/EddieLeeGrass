#pragma once

#pragma comment(lib, "winmm.lib")


#include <windows.h>
#include <mmsystem.h>


class CFPS
{
public:
	CFPS();
	CFPS(const CFPS&);
	~CFPS();

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_fps, m_count;
	unsigned long m_startTime;
};