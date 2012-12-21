#pragma once

#pragma comment(lib, "pdh.lib")

#include <pdh.h>

class CCPU
{
public:
	CCPU();
	CCPU(const CCPU&);
	~CCPU();

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();
	unsigned long GetTime();

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};