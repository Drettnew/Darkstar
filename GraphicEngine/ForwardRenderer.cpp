#include "ForwardRenderer.h"

ForwardRenderer::ForwardRenderer()
{
	m_Shader = 0;
}

ForwardRenderer::~ForwardRenderer()
{
}

bool ForwardRenderer::Initialize(ID3D11Device* device, HWND hwnd, int numLights)
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

	m_StructuredLightBuffer.Initialize(numLights, sizeof(Light), true, false, NULL, device);
	m_StructuredLightBuffer.InitializeResourceView(device);

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

bool ForwardRenderer::Render(D3D* directX, Camera* camera, Model* model, LightList* lights)
{
	bool result;
	HRESULT hr;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Light* dataPtr;

	//Lock the light constant buffer so it can be written to
	hr = directX->GetDeviceContext()->Map(m_StructuredLightBuffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	dataPtr = (Light*)mappedResource.pData;

	std::vector<Light> lightVector = lights->GetLightList();

	for (int i = 0; i < lightVector.size(); i++)
	{
		Light light = lightVector[i];

		//Copy the lighting variables into the constant buffer
		dataPtr[i].m_Color = light.m_Color;

		dataPtr[i].m_DirectionWS = light.m_DirectionWS;
		dataPtr[i].m_DirectionVS = light.m_DirectionVS;

		dataPtr[i].m_PositionWS = light.m_PositionWS;
		dataPtr[i].m_PositionVS = light.m_PositionVS;

		dataPtr[i].m_Range = light.m_Range;
		dataPtr[i].m_Intensity = light.m_Intensity;
		dataPtr[i].m_Enabled = light.m_Enabled;
		dataPtr[i].m_SpotlightAngle = light.m_SpotlightAngle;
		dataPtr[i].m_Type = light.m_Type;
	}

	//Unlock the constant buffer
	directX->GetDeviceContext()->Unmap(m_StructuredLightBuffer.GetBuffer(), 0);

	//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
	model->Render(directX->GetDeviceContext());

	directX->GetDeviceContext()->PSSetShaderResources(1, 1, m_StructuredLightBuffer.GetResourceView());

	result = m_Shader->Render(directX->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix,
		model->GetTexture(), camera->GetPosition());
	if (!result)
	{
		return false;
	}

	return true;
}

bool ForwardRenderer::MultiModelRender(D3D * directX, Camera * camera, Model * model, LightList * lights, Model * multiModel, XMMATRIX * worldMatrices, int numModels)
{
	bool result;
	HRESULT hr;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Light* dataPtr;

	//Lock the light constant buffer so it can be written to
	hr = directX->GetDeviceContext()->Map(m_StructuredLightBuffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	dataPtr = (Light*)mappedResource.pData;

	std::vector<Light> lightVector = lights->GetLightList();

	for (int i = 0; i < lightVector.size(); i++)
	{
		Light light = lightVector[i];

		//Copy the lighting variables into the constant buffer
		dataPtr[i].m_Color = light.m_Color;

		dataPtr[i].m_DirectionWS = light.m_DirectionWS;
		dataPtr[i].m_DirectionVS = light.m_DirectionVS;

		dataPtr[i].m_PositionWS = light.m_PositionWS;
		dataPtr[i].m_PositionVS = light.m_PositionVS;

		dataPtr[i].m_Range = light.m_Range;
		dataPtr[i].m_Intensity = light.m_Intensity;
		dataPtr[i].m_Enabled = light.m_Enabled;
		dataPtr[i].m_SpotlightAngle = light.m_SpotlightAngle;
		dataPtr[i].m_Type = light.m_Type;
	}

	//Unlock the constant buffer
	directX->GetDeviceContext()->Unmap(m_StructuredLightBuffer.GetBuffer(), 0);
	directX->GetDeviceContext()->PSSetShaderResources(1, 1, m_StructuredLightBuffer.GetResourceView());

	//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
	model->Render(directX->GetDeviceContext());

	result = m_Shader->Render(directX->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix,
		model->GetTexture(), camera->GetPosition());
	if (!result)
	{
		return false;
	}

	multiModel->Render(directX->GetDeviceContext());

	for (int i = 0; i < numModels; i++)
	{
		result = m_Shader->Render(directX->GetDeviceContext(), multiModel->GetIndexCount(), worldMatrices[i], viewMatrix, projectionMatrix,
			multiModel->GetTexture(), camera->GetPosition());
		if (!result)
		{
			return false;
		}
	}

	return true;
}
