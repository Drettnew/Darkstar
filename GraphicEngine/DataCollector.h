#include <chrono>
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>
#include<fstream>

#include "Util.h"

#pragma once
using namespace std::chrono;
using namespace std;
class DataCollector
{
public:
	static DataCollector& GetInstance()
	{
		//if (!s_instance)
		//{
		//	s_instance = new DataCollector;
		//}
		//return s_instance;

		static DataCollector instance;
		return instance;
	}

	void StartTimer();
	void UpdateTimer();
	void EndTimer();

	void Reset();

	float GetResult();
	int GetFps();

	void GenerateLog();

private:
	const static int NUM_SAMPLES = 10;
	int currentResultIndex = 0;
	int currentFPSIndex = 0;
	int counter = 0;

	//static DataCollector *s_instance;

	high_resolution_clock::time_point startTime;
	high_resolution_clock::time_point endTime;

	int m_fps, m_count;
	unsigned long m_startTime;

	float timeResult = 0;
	int fps = 0;

	float resultArray[NUM_SAMPLES];
	int fpsArray[NUM_SAMPLES];

	DataCollector();
	~DataCollector();

	DataCollector(DataCollector const& copy);
};