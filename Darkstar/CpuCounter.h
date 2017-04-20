#pragma once

#pragma comment(lib, "pdh.lib")

#include <Pdh.h>

class CpuCounter
{
public:
	CpuCounter();
	~CpuCounter();

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};