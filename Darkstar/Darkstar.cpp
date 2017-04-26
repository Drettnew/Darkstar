#include "Darkstar.h"



bool Darkstar::Frame()
{
	bool result, keyDown;
	float rotationY;
	int mouseX, mouseY;
	//Update the system stats
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Do the input frame processing.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	//Do the frame processing for the graphics object.
	result = m_Graphics->Frame(m_Input->GetCameraInputs(), m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime());
	if (!result)
	{
		return false;
	}

	return true;
}

void Darkstar::InitializeWindows(int & screenWidth, int & screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScrenSettings;
	int posX, posY;

	//Get an external pointer to this object
	ApplicationHandle = this;

	//Get the instance of this application
	m_hinstance = GetModuleHandle(NULL);

	//Give the application a name
	m_applicationName = L"Darkstar";

	//Setup the windows class with default settings
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hinstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= m_applicationName;
	wc.cbSize			= sizeof(WNDCLASSEX);

	//Register the window class
	RegisterClassEx(&wc);

	//Determine the resolution of the clients desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Setup the sceen setting depending on whether it is running in full scren or in windowed mode
	if (FULL_SCREEN)
	{
		//If full screen set the screen to the maximum size of the users desktop and 32bit.
		memset(&dmScrenSettings, 0, sizeof(dmScrenSettings));
		dmScrenSettings.dmSize			= sizeof(dmScrenSettings);
		dmScrenSettings.dmPelsWidth		= (unsigned long)screenWidth;
		dmScrenSettings.dmPelsHeight	= (unsigned long)screenHeight;
		dmScrenSettings.dmBitsPerPel	= 32;
		dmScrenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change the display setting to full screen.
		ChangeDisplaySettings(&dmScrenSettings, CDS_FULLSCREEN);

		//Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		//If windowed then set it to 800x600 resolution
		screenWidth = 800;
		screenHeight = 600;

		//Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with the scren settings and get handle to it
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//Hide the mouse cursor
	ShowCursor(false);
}

void Darkstar::ShutdownWindows()
{
	//Show the mouse cursor
	ShowCursor(true);

	//Fix the dispaly setting if leaving full screen mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//Remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//Remove the application instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//Realse the pointer to this class
	ApplicationHandle = NULL;
}

Darkstar::Darkstar()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_Timer = 0;
}

Darkstar::Darkstar(const Darkstar& other)
{
}

Darkstar::~Darkstar()
{
	OutputDebugString(L"Dude---------------------------------------------------\n");
}

void Darkstar::Run()
{
	MSG msg;
	bool done, result;

	//Init message structure
	ZeroMemory(&msg, sizeof(MSG));

	//Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		//Handlw the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//if windows signals to end the application then exit out
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//Otherwise do the frame processing
			result = Frame();
			if (!result)
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				done = true;
			}
		}

		//Check if the user pressed escape and want to quit
		if (m_Input->IsKeyHit(DIK_ESCAPE) == true)
		{
			done = true;
		}
	}
}

void Darkstar::Shutdown()
{

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the cpu object.
	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	//Release the graphics object
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//Realease the input object
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	//Shutdown the window
	ShutdownWindows();
}

bool Darkstar::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	//Init windows api
	InitializeWindows(screenWidth, screenHeight);

	//Create input object. This will handle reading the keyboard input from the user
	m_Input = new Input;
	if (!m_Input)
		return false;

	//Init input object
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	//Create graphic object. This will handle rendering all the graphics for this application
	m_Graphics = new Graphics;
	if (!m_Graphics)
		return false;

	//Init graphic object
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
		return false;

	// Create the fps object.
	m_Fps = new FpsCounter;
	if (!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CpuCounter;
	if (!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the timer object.
	m_Timer = new Timer;
	if (!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

LRESULT Darkstar::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//Any other messages send to the default message handler
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}

	return LRESULT();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
