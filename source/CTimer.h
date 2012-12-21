#pragma once

#include <windows.h>


class CTimer
{
public:
	CTimer();
	CTimer(const CTimer&);
	~CTimer();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};