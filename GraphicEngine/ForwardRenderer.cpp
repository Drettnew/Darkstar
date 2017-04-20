#include "ForwardRenderer.h"

ForwardRenderer::ForwardRenderer()
{
	m_Shader = 0;
	m_frameLights = 0;
	m_frameModels = 0;
	m_Model = 0;
}

ForwardRenderer::~ForwardRenderer()
{
}

bool ForwardRenderer::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Create the light shader object.
	m_Shader = new LightShader;
	if (!m_Shader)
	{
		return false;
	}


	// Initialize the texture shader object.
	result = m_Shader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ForwardRenderer::Shutdown()
{
	// Release the color shader object.
	if (m_Shader)
	{
		m_Shader->Shutdown();
		delete m_Shader;
		m_Shader = 0;
	}
}

bool ForwardRenderer::SetupData(ModelList* modelList, Light* light, ModelAsset* model)
{
	m_frameModels = modelList;
	m_frameLights = light;
	m_Model = model;

	return true;
}

bool ForwardRenderer::Render(D3D* directX, Camera* camera)
{
	bool result;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float positionX, positionY, positionZ, radius;

	for (int i = 0; i < m_frameModels->GetModelCount(); i++)
	{

	}
	//Get the view and projection matrices
	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	for (int index = 0; index < m_frameModels->GetModelCount(); index++)
	{
		//Reset the view matrix for the next model
		directX->GetWorldMatrix(worldMatrix);

		//Move the model to the location it should be rendered at
		worldMatrix = worldMatrix * XMMatrixTranslation(positionX, positionY, positionZ);

		//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
		m_Model->Render(directX->GetDeviceContext());

		////Render the model using the light shader
		//result = m_Shader->Render(directX->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		//	m_Model->GetTexture(), m_frameLights->GetDirection(), m_frameLights->GetDiffuseColor(), m_frameLights->GetAmbientColor(),
		//	camera->GetPosition(), m_frameLights->GetSpecularColor(), m_frameLights->GetSpecularPower());
		//if (!result)
		//{
		//	return false;
		//}
	}

	return true;
}
