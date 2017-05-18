#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include "Camera.h"
#include "Graphics.h"

class Input
{
private:
	const static int s_NumKeys = 256;

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	DIMOUSESTATE m_mouseState;

	bool m_keys[s_NumKeys];
	bool m_prevKeys[s_NumKeys];

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
	int m_mouseIX, m_mouseIY;

	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();
public:
	Input();
	Input(const Input& other);
	~Input();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();
	bool Frame();

	Camera::CameraInputType GetCameraInputs();
	Graphics::GraphicInput GetGraphicInputs();

	bool IsKeyDown(unsigned int key);
	bool IsKeyHit(unsigned int key);
	void GetMouseLocation(int& mouseX, int& mouseY);
};