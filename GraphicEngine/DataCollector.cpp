#include "DataCollector.h"

void DataCollector::StartTimer()
{
	startTime = high_resolution_clock::now();
}

void DataCollector::UpdateTimer()
{
	m_count++;
	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();

		if (currentFPSIndex < NUM_SAMPLES)
		{
			fpsArray[currentFPSIndex] = m_fps;
			currentFPSIndex++;
		}
		else
		{
			currentFPSIndex = 0;

			fpsArray[currentFPSIndex] = m_fps;
			currentFPSIndex++;
		}

	}
}

void DataCollector::EndTimer()
{
	endTime = high_resolution_clock::now();

	timeResult = duration_cast<microseconds>(endTime - startTime).count();

	if (currentResultIndex < NUM_SAMPLES)
	{
		resultArray[currentResultIndex] = duration_cast<microseconds>(endTime - startTime).count();
		currentResultIndex++;
	}
	else
	{
		currentResultIndex = 0;

		resultArray[currentResultIndex] = duration_cast<microseconds>(endTime - startTime).count();
		currentResultIndex++;
	}
}

void DataCollector::Reset()
{
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		fpsArray[i] = 0;
	}
	currentFPSIndex = 0;
	m_fps = 0;
	m_count = 0;

	m_startTime = timeGetTime();
}

float DataCollector::GetResult()
{
	timeResult = resultArray[currentResultIndex];

	return timeResult;
}

int DataCollector::GetFps()
{
	return m_fps;
}

void DataCollector::GenerateLog()
{
	ofstream fout;

	time_t currentTime = time(0);
	tm* currentDate = localtime(&currentTime);
	char buffer[80];
	strftime(buffer, 80, "log-%Y-%m-%d_%H-%M-%S.txt", currentDate);

	// Open a file to write the error message to.
	fout.open(buffer);

	fout << "FPS: \n";
	int averageFPS = 0;
	for (int i = 0; i < currentFPSIndex; i++)
	{
		fout << fpsArray[i] << std::endl;

		averageFPS += fpsArray[i];
	}

	fout << "Average FPS: ";
	fout << averageFPS / currentFPSIndex << std::endl;;

	// Close the file.
	fout.close();
}

DataCollector::DataCollector()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
}

DataCollector::~DataCollector()
{
}

DataCollector::DataCollector(DataCollector const & copy)
{
}
