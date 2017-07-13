#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include "Graphics.h"
#include "Input.h"
#include "resource.h"

#include "FpsCounter.h"
#include "CpuCounter.h"
#include "Timer.h"

class Darkstar
{
private:
	const static int SCREEN_HEIGHT = 720;
	const static int SCREEN_WIDTH = 1280;

	int m_screenResolutionX;
	int m_screenResolutionY;

	bool mouseMoved = false;

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_Input;
	Graphics* m_Graphics;

	FpsCounter* m_Fps;
	CpuCounter* m_Cpu;
	Timer* m_Timer;

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

