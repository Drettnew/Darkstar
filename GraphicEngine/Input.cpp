#include "Input.h"

Input::Input() {
	memset(m_keys, 0, sizeof(bool) * s_NumKeys);
	memset(m_mouseKeys, 0, sizeof(bool) * s_NumMouseKeys);
}
Input::Input(const Input & other)
{
}
Input::~Input() {

}

bool Input::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	//Store the screen size
	m_screenHeight = screenHeight;
	m_screenWidth = screenWidth;

	//Initialize the location of the mouse
	m_mouseX = 0;
	m_mouseY = 0;

	m_mouseIX = 0;
	m_mouseIY = 0;

	m_mouseLX = 0;
	m_mouseLY = 0;

	return true;

}

void Input::Shutdown()
{
}

bool Input::Frame()
{
	memcpy(m_prevKeys, m_keys, sizeof(bool) * s_NumKeys);
	memcpy(m_prevMouseKeys, m_mouseKeys, sizeof(bool) * s_NumMouseKeys);

	if (m_mouseIX == m_mousePrevIX)
	{
		m_mouseIX = 0;
	}
	else
	{
		m_mousePrevIX = m_mouseIX;
	}

	if (m_mouseIY == m_mousePrevIY)
	{
		m_mouseIY = 0;
	}
	else
	{
		m_mousePrevIY = m_mouseIY;
	}

	return true;
}

Camera::CameraInputType Input::GetCameraInputs()
{
	Camera::CameraInputType output;

	output.back = IsKeyDown(Keys::S);
	output.forward = IsKeyDown(Keys::W);
	output.left = IsKeyDown(Keys::A);
	output.right = IsKeyDown(Keys::D);

	output.mouseX = m_mouseX;
	output.mouseY = m_mouseY;

	output.mouseIX = m_mouseIX;
	output.mouseIY = m_mouseIY;

	output.rightMouseButton = IsMouseKeyDown(MouseKeys::MOUSE_RIGHT);

	if (IsKeyDown(Keys::B))
	{
		int i = 10000;
	}

	return output;
}

Graphics::GraphicInput Input::GetGraphicInputs()
{
	Graphics::GraphicInput output;
	output.GPURender = IsKeyDown(Keys::NUMPAD_1);
	output.CPURender = IsKeyDown(Keys::NUMPAD_2);
	output.ForwardRender = IsKeyDown(Keys::NUMPAD_3);

	output.GPURenderMulti = IsKeyDown(Keys::NUMPAD_4);
	output.CPURenderMulti = IsKeyDown(Keys::NUMPAD_5);
	output.ForwardRenderMulti = IsKeyDown(Keys::NUMPAD_6);

	return output;
}

void Input::KeyDown(unsigned int key)
{
	m_keys[key] = true;
}

void Input::KeyUp(unsigned int key)
{
	m_keys[key] = false;
}

void Input::MouseKeyDown(unsigned int key)
{
	m_mouseKeys[key] = true;
}

void Input::MouseKeyUp(unsigned int key)
{
	if (m_prevMouseKeys[key])
	{
		m_mouseKeys[key] = false;
	}
}

bool Input::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}

bool Input::IsKeyHit(unsigned int key)
{
	return m_keys[key] && !m_prevKeys[key];
}

bool Input::IsMouseKeyDown(unsigned int key)
{
	return m_mouseKeys[key];
}

void Input::MouseMove(int xPos, int yPos)
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX = xPos;
	m_mouseY = yPos;

	m_mousePrevIX = m_mouseIX;
	m_mousePrevIY = m_mouseIY;

	//Save delta movement
	m_mouseIX = (float)m_mouseX - (float)m_mouseLX;
	m_mouseIY = (float)m_mouseY - (float)m_mouseLY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	m_mouseLX = m_mouseX;
	m_mouseLY = m_mouseY;
}


void Input::GetMouseLocation(int & mouseX, int & mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}
