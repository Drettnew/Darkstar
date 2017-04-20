#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Graphics.h"
#include "Input.h"

#include "FpsCounter.h"
#include "CpuCounter.h"
#include "Timer.h"
#include "Position.h"

class Darkstar
{
private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_Input;
	Graphics* m_Graphics;

	FpsCounter* m_Fps;
	CpuCounter* m_Cpu;
	Timer* m_Timer;
	Position* m_Position;

	bool Frame();
	void InitializeWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();
public:
	Darkstar();
	Darkstar(const Darkstar& other);
	~Darkstar();

	void Run();
	void Shutdown();
	bool Initialize();
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
};

//Function prototypes
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

//Globals
static Darkstar* ApplicationHandle = 0;

