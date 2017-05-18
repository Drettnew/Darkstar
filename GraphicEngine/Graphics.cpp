#include "Graphics.h"

//DataCollector *DataCollector::s_instance = 0;

Graphics::Graphics() {
	m_Direct3D = 0;
	m_Camera = 0;
	m_LightShader = 0;
	m_Text = 0;
	m_Frustum = 0;
	//m_Assets = 0;
	m_Renderer = 0;
	m_ForwardRender = 0;
	m_CpuRenderer = 0;
	m_model = 0;
	m_sphereModel = 0;
}
Graphics::Graphics(const Graphics & other)
{
}
Graphics::~Graphics() {

}

bool Graphics::Initialize(int & width, int & height, HWND hwnd)
{
	bool result;
	
	//Create the Direct3D object
	m_Direct3D = new D3D;
	if (!m_Direct3D)
		return false;

	//Init the Direct3D object
	result = m_Direct3D->Initialize(width, height, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//Create the Assets object
	//m_Assets = new Assets;
	//if (!m_Assets)
	//{
	//	return false;
	//}

	//Initialize the assets object
	//m_Assets->bind(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

	// Create the camera object.
	m_Camera = new Camera;
	if (!m_Camera)
	{
		return false;
	}

	//Init a base view matrix with the camera for 2d user interface rendering
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();

	// Create the text object.
	m_Text = new Text;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, width, height);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}


	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 30.0f, -50.0f);

	m_model = new Model();

	//TEMP____________________
	m_model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Data/Models/floor.obj");

	m_lightList.Random(50, 50, NUM_LIGHTS);

	m_sphereModel = new Model();

	m_sphereModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Data/Models/sphere.obj");

	//modelList.Random(m_Assets, 5, "../Data/Models/sphere.obj", 10, 10, false);

	//// Create the texture shader object.
	//m_TextureShader = new TextureShader;
	//if (!m_TextureShader)
	//{
	//	return false;
	//}

	//// Initialize the texture shader object.
	//result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
	//	return false;
	//}

	// Create the light shader object.
	m_LightShader = new LightShader;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	////Create the bitmap object
	//m_Bitmap = new Bitmap;
	//if (!m_Bitmap)
	//{
	//	return false;
	//}

	////Initialize the bitmap object
	//result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), width, height, "../Data/stone01.tga", 100, 100);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize th bitmap boject", L"Error", MB_OK);
	//	return false;
	//}

	////Create the light Object
	//m_Light = new Light;
	//if (!m_Light)
	//{
	//	return false;
	//}

	////Initialize the light object
	//m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	//m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	//m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetSpecularPower(32.0f);

	////Create the model list object
	//m_ModelList = new ModelList;
	//if (!m_ModelList)
	//{
	//	return false;
	//}

	////Initialize th model list object
	//result = m_ModelList->Initialize(1);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
	//	return false;
	//}

	////Create the frustum object
	//m_Frustum = new Frustum;
	//if (!m_Frustum)
	//{
	//	return false;
	//}

	//Create the renderer object
	m_Renderer = new ForwardPlusGPU;
	if (!m_Renderer)
	{
		return false;
	}

	//Initialize the renderer object
	result = m_Renderer->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, NUM_LIGHTS);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the renderer object", L"Error", MB_OK);
		return false;
	}

	//Create the renderer object
	m_CpuRenderer = new ForwardPlusCPU;
	if (!m_CpuRenderer)
	{
		return false;
	}

	//Initialize the renderer object
	result = m_CpuRenderer->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, NUM_LIGHTS);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the renderer object", L"Error", MB_OK);
		return false;
	}

	//Create the renderer object
	m_ForwardRender = new ForwardRenderer;
	if (!m_ForwardRender)
	{
		return false;
	}

	//Initialize the renderer object
	result = m_ForwardRender->Initialize(m_Direct3D->GetDevice(), hwnd, NUM_LIGHTS);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the renderer object", L"Error", MB_OK);
		return false;
	}

	//Seed the random generator with the current time
	srand((unsigned int)time(NULL));

	for (int i = 0; i < NUM_MODEL; i++)
	{
		float x = (-50) + static_cast <float> (rand()) / (static_cast<float> (RAND_MAX / (50 + 50)));

		float z = (-50) + static_cast <float> (rand()) / (static_cast<float> (RAND_MAX / (50 + 50)));

		worldMatrices[i] = XMMatrixTranslation(x, 1, z);
	}

	return true;
}

bool Graphics::Frame(Camera::CameraInputType input, GraphicInput gInput, int fps, int cpu, float frameTime, bool logData, bool resetData)
{
	DataCollector::GetInstance().UpdateTimer();

	if (logData)
	{
		DataCollector::GetInstance().GenerateLog();
	}

	if (resetData)
	{
		DataCollector::GetInstance().Reset();
	}

	if (gInput.GPURender)
	{
		renderMode = 0;
	}
	else if (gInput.CPURender)
	{
		renderMode = 1;
	}
	else if (gInput.ForwardRender)
	{
		renderMode = 2;
	}
	else if (gInput.GPURenderMulti)
	{
		renderMode = 3;
	}
	else if (gInput.CPURenderMulti)
	{
		renderMode = 4;
	}
	else if (gInput.ForwardRenderMulti)
	{
		renderMode = 5;
	}

	bool result;

	static float rotation = 0.0f;

	m_Text->QueueString("wubba lubba dub dub", 20, 60, 0.0f, 1.0f, 0.0f, m_Direct3D->GetDeviceContext());

	//Run assets reference checks
	//m_Assets->upload();
	//m_Assets->checkReferences();

	//Update camera input
	m_Camera->HandleInputs(input, frameTime);

	//Update the rotation variable each frame
	//rotation += ((float)XM_PI / 5000.0f) * frameTime;
	//if (rotation > 360.0f)
	//{
	//	rotation -= 360.0f;
	//}

	//model.SetRotation(0.0f, 0.0f, 0.0f);

	// Set the frames per second.
	result = m_Text->SetFps(fps, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Set the cpu usage.
	result = m_Text->QueueString("Cpu: " + std::to_string(cpu) + "%", 20, 40, 0.0f, 1.0f, 0.0f, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if (!result)
	{
		return false;
	}
	//m_Assets->checkHotload(frameTime);
}

void Graphics::Shutdown()
{
	/*if (m_Assets)
	{
		delete m_Assets;
		m_Assets = 0;
	}*/

	//Release the text object
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the frustum object
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	//Remove renderer
	if (m_Renderer)
	{
		m_Renderer->Shutdown();
		delete m_Renderer;
		m_Renderer = 0;
	}

	//Remove renderer
	if (m_CpuRenderer)
	{
		m_CpuRenderer->Shutdown();
		delete m_CpuRenderer;
		m_CpuRenderer = 0;
	}

	if (m_ForwardRender)
	{
		m_ForwardRender->Shutdown();
		delete m_ForwardRender;
		m_ForwardRender = 0;
	}

	if (m_model)
	{
		m_model->Shutdown();
		delete m_model;
		m_model = 0;
	}

	if (m_sphereModel)
	{
		m_sphereModel->Shutdown();
		delete m_sphereModel;
		m_sphereModel = 0;
	}

}

bool Graphics::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	//XMFLOAT4 color;
	bool result = true;
	//bool renderModel;
	//int modelCount, renderCount, index;
	//float positionX, positionY, positionZ, radius;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Generate the view matrix based on camera's position
	m_Camera->Render();

	//Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	m_lightList.UpdateLights(viewMatrix);

	//DataCollector::GetInstance()->StartTimer();

	

	//DataCollector::GetInstance()->EndTimer();

	m_Text->QueueString(std::to_string(DataCollector::GetInstance().GetFps()), 20, 80, 0.0f, 1.0f, 0.0f, m_Direct3D->GetDeviceContext());

	if (renderMode == 0)
	{
		m_Renderer->Render(m_Direct3D, m_Camera, m_model, &m_lightList);
	}
	else if (renderMode == 1)
	{
		m_CpuRenderer->Render(m_Direct3D, m_Camera, m_model, &m_lightList);
	}
	else if (renderMode == 2)
	{
		m_ForwardRender->Render(m_Direct3D, m_Camera, m_model, &m_lightList);
	}
	else if (renderMode == 3)
	{
		m_Renderer->MultiModelRender(m_Direct3D, m_Camera, m_model, &m_lightList, m_sphereModel, worldMatrices, NUM_MODEL);
	}
	else if (renderMode == 4)
	{
		m_CpuRenderer->MultiModelRender(m_Direct3D, m_Camera, m_model, &m_lightList, m_sphereModel, worldMatrices, NUM_MODEL);
	}
	else if (renderMode == 5)
	{
		m_ForwardRender->MultiModelRender(m_Direct3D, m_Camera, m_model, &m_lightList, m_sphereModel, worldMatrices, NUM_MODEL);
	}

	//m_CpuRenderer->Render(m_Direct3D, m_Camera, m_model, &m_lightList);

	////Contstruct the frustum
	//m_Frustum->ConstructFrustum(projectionMatrix, viewMatrix);

	////Get the number of models thar will be rendered
	//modelCount = m_ModelList->GetModelCount();

	////Initialize the count of models that have been rendered
	//renderCount = 0;

	////Go through all the models and render them only if they can be seen by the camera view
	//for (index = 0; index < modelCount; index++)
	//{
	//	//Get the postion and color of the spehere model at this index
	//	m_ModelList->GetData(index, positionX, positionY, positionZ, color);

	//	//Set the radius of the sphere to 1.0;
	//	radius = 1.0f;

	//	//Check if the sphere model is in the view frustum
	//	renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);

	//	//If it can be seen then render it if not skip this model and check the next sphere
	//	if (true)
	//	{

	//	}
	//}

	//Turn of the Z buffer to begin all 2D rendering
	m_Direct3D->TurnZBufferOff();

	//Put the bitmap vertex and index buffer on the graphics pipeling to prepeare them for rendering
	//result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 325, 25);
	if (!result)
	{
		return false;
	}

	////Get a new world matrix
	m_Direct3D->GetWorldMatrix(worldMatrix);

	//Render the bitmap with the texture shader
	//result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, m_Camera->GetBaseViewMatrix(), orthoMatrix, m_Bitmap->GetTexture());
	if (!result)
	{
		return false;
	}

	//Turn on the alpha blending before rendring text
	m_Direct3D->TurnOnAlphaBlending();

	//Render the text strings
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	//Turn off alpha blendinng after rendering the text
	m_Direct3D->TurnOffAlphaBlending();

	//Turn the Z buffer back on now that all 2D rendering has completed
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();
	return true;
}
