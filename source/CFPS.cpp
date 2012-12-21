#include "CFPS.h"


CFPS::CFPS()
{
} 


CFPS::CFPS(const CFPS& other)
{
}


CFPS::~CFPS()
{
}


void CFPS::Initialize()
{
	// Initialize the counters and the start time.
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	
	return;
}


void CFPS::Frame()
{
	m_count++;

	// If one second has passed then update the frame per second speed.
	if(timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;
		
		m_startTime = timeGetTime();
	}
}


int CFPS::GetFps()
{
	return m_fps;
}