#include "Graphics.h"

Graphics::Graphics() {
	m_Direct3D = 0;
	m_Camera = 0;
	m_LightShader = 0;
	m_Text = 0;
	m_Frustum = 0;
	m_ModelList = 0;
	//m_Assets = 0;
	m_Renderer = 0;
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
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	m_model = new Model();

	//TEMP____________________
	m_model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Data/Models/floor.obj");

	m_lightList.Random(10, 10, 4);

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
	result = m_Renderer->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the renderer object", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Graphics::Frame(Camera::CameraInputType input, int fps, int cpu, float frameTime)
{
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

	// Release the modellist object
	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	//Remove renderer
	if (m_Renderer)
	{
		m_Renderer->Shutdown();
		delete m_Renderer;
		m_Renderer = 0;
	}

	if (m_model)
	{
		m_model->Shutdown();
		delete m_model;
		m_model = 0;
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
	//		m_Direct3D->GetWorldMatrix(worldMatrix);

	//		//Rotate the world matrix by the rotation value so that the tiangle will spin
	//		worldMatrix = worldMatrix * XMMatrixRotationY(rotation);

	//		//Move the model to the location it should be rendered at
	//		worldMatrix = worldMatrix * XMMatrixTranslation(positionX, positionY, positionZ);

			//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
			m_model->Render(m_Direct3D->GetDeviceContext());

			//Render the model using the light shader
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_model->GetIndexCount(), m_model->GetWorldMatrix(), viewMatrix, projectionMatrix,
				m_model->GetTexture(),
				m_Camera->GetPosition(), &m_lightList);
			if (!result)
			{
				return false;
			}

	//		//Since this model was rendered increment the count
	//		renderCount++;
	//	}
	//}

			//for (int i = 0; i < modelList.GetModelCount(); i++)
			//{
			//	//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
			//	modelList.GetModel(i)->Render(m_Direct3D->GetDeviceContext());

			//	//Render the model using the light shader
			//	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), modelList.GetModel(i)->GetIndexCount(), modelList.GetModel(i)->GetWorldMatrix(), viewMatrix, projectionMatrix,
			//		modelList.GetModel(i)->GetTexture(),
			//		m_Camera->GetPosition());
			//	if (!result)
			//	{
			//		return false;
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
