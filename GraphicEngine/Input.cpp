#include "Input.h"

bool Input::ReadKeyboard()
{
	HRESULT result;

	//Read the keyboard device
	result = m_keyboard->GetDeviceState(sizeof(m_keys), (LPVOID)&m_keys);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool Input::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Input::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	//Save delta movement
	m_mouseIX = m_mouseState.lX;
	m_mouseIY = m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
}

Input::Input() {
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
	memset(m_keys, 0, sizeof(bool) * s_NumKeys);
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

	//Initialize the main direct input interface
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;

}

void Input::Shutdown()
{
	// Release the mouse.
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool Input::Frame()
{
	memcpy(m_prevKeys, m_keys, sizeof(bool) * s_NumKeys);

	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

Camera::CameraInputType Input::GetCameraInputs()
{
	Camera::CameraInputType output;

	output.back = IsKeyDown(DIK_DOWNARROW);
	output.forward = IsKeyDown(DIK_UPARROW);
	output.left = IsKeyDown(DIK_LEFTARROW);
	output.right = IsKeyDown(DIK_RIGHTARROW);

	output.mouseX = m_mouseX;
	output.mouseY = m_mouseY;

	output.mouseIX = m_mouseIX;
	output.mouseIY = m_mouseIY;

	output.rightMouseButton = IsKeyDown(DIK_E);

	return output;
}

Graphics::GraphicInput Input::GetGraphicInputs()
{
	Graphics::GraphicInput output;
	output.GPURender = IsKeyDown(DIK_NUMPAD1);
	output.CPURender = IsKeyDown(DIK_NUMPAD2);
	output.ForwardRender = IsKeyDown(DIK_NUMPAD3);

	output.GPURenderMulti = IsKeyDown(DIK_NUMPAD4);
	output.CPURenderMulti = IsKeyDown(DIK_NUMPAD5);
	output.ForwardRenderMulti = IsKeyDown(DIK_NUMPAD6);

	return output;
}

bool Input::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}

bool Input::IsKeyHit(unsigned int key)
{
	return m_keys[key] && !m_prevKeys[key];
}


void Input::GetMouseLocation(int & mouseX, int & mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}
