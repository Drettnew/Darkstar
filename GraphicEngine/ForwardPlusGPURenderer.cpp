#include "ForwardPlusGPURenderer.h"

ForwardPlusGPU::ForwardPlusGPU()
{
	m_Shader = 0;
}

ForwardPlusGPU::~ForwardPlusGPU()
{
}

bool ForwardPlusGPU::Initialize(ID3D11Device * device, HWND hwnd)
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

	m_lightList.Random(10, 10, 4);

	return true;
}

void ForwardPlusGPU::Shutdown()
{
	// Release the color shader object.
	if (m_Shader)
	{
		m_Shader->Shutdown();
		delete m_Shader;
		m_Shader = 0;
	}
}

bool ForwardPlusGPU::Render(D3D * directX, Camera * camera, Model* model)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
	model->Render(directX->GetDeviceContext());

	//Render the model using the light shader
	result = m_Shader->Render(directX->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix,
									model->GetTexture(), camera->GetPosition(), &m_lightList);
	if (!result)
	{
		return false;
	}

	return true;
}
