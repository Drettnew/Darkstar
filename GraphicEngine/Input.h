#pragma once

#include "Camera.h"
#include "Graphics.h"

class Input
{
private:
	const static int s_NumKeys = 256;
	const static int s_NumMouseKeys = 3;

	bool m_keys[s_NumKeys];
	bool m_prevKeys[s_NumKeys];

	bool m_mouseKeys[s_NumMouseKeys];
	bool m_prevMouseKeys[s_NumKeys];

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
	float m_mouseIX, m_mouseIY;
	float m_mousePrevIX, m_mousePrevIY;
	int m_mouseLX, m_mouseLY;
public:
	Input();
	Input(const Input& other);
	~Input();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();
	bool Frame();

	Camera::CameraInputType GetCameraInputs();
	Graphics::GraphicInput GetGraphicInputs();

	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);

	void MouseKeyDown(unsigned int key);
	void MouseKeyUp(unsigned int key);

	bool IsKeyDown(unsigned int key);
	bool IsKeyHit(unsigned int key);

	bool IsMouseKeyDown(unsigned int key);

	void MouseMove(int xPos, int yPos);

	void GetMouseLocation(int& mouseX, int& mouseY);
};

static enum Keys
{
	A = 0x41,
	B = 0x42,
	C = 0x43,
	D = 0x44,
	E = 0x45,
	F = 0x46,
	G = 0x47,
	H = 0x48,
	I = 0x49,
	J = 0x4A,
	K = 0x4B,
	L = 0x4C,
	M = 0x4D,
	N = 0x4E,
	O = 0x4F,
	P = 0x50,
	Q = 0x51,
	R = 0x52,
	S = 0x53,
	T = 0x54,
	U = 0x55,
	V = 0x56,
	W = 0x57,
	X = 0x58,
	Y = 0x59,
	Z = 0x5A,

	ESCAPE = VK_ESCAPE,
	NUMPAD_0 = VK_NUMPAD0,
	NUMPAD_1 = VK_NUMPAD1,
	NUMPAD_2 = VK_NUMPAD2,
	NUMPAD_3 = VK_NUMPAD3,
	NUMPAD_4 = VK_NUMPAD4,
	NUMPAD_5 = VK_NUMPAD5,
	NUMPAD_6 = VK_NUMPAD6,
	NUMPAD_7 = VK_NUMPAD7,
	NUMPAD_8 = VK_NUMPAD8,
	NUMPAD_9 = VK_NUMPAD9,

	NUM_KEYS
};

enum MouseKeys
{
	MOUSE_LEFT = 0,
	MOUSE_MIDDLE = 1,
	MOUSE_RIGHT = 2,

	NUM_MOUSE_KEYS
};