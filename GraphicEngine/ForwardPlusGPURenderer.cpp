#include "ForwardPlusGPURenderer.h"

ForwardPlusGPU::ForwardPlusGPU()
{
	m_Shader = 0;
	m_depthPrePass = 0;
	m_FrustumCS = 0;
	m_CullingCS = 0;
}

ForwardPlusGPU::~ForwardPlusGPU()
{
}

bool ForwardPlusGPU::Initialize(ID3D11Device * device, ID3D11DeviceContext* deviceContext, HWND hwnd, int numLights)
{
	bool result;

	// Create the light shader object.
	m_Shader = new ForwardPlusLightShader;
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

	m_depthPrePass = new DepthPass;
	if (!m_depthPrePass)
	{
		return false;
	}

	result = m_depthPrePass->Initialize(device, deviceContext, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the depth pre-pass object.", L"Error", MB_OK);
		return false;
	}

	m_FrustumCS = new FrustumComputeShader;
	if (!m_FrustumCS)
	{
		return false;
	}

	result = m_FrustumCS->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the frustum compute shader object.", L"Error", MB_OK);
		return false;
	}

	m_CullingCS = new CullingComputeShader;
	if (!m_CullingCS)
	{
		return false;
	}

	result = m_CullingCS->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the culling compute shader object.", L"Error", MB_OK);
		return false;
	}

	//m_lightList.Random(50, 50, NUM_LIGHTS);

	m_StructuredLightBuffer.Initialize(numLights, sizeof(Light), true, false, NULL, device);
	m_StructuredLightBuffer.InitializeResourceView(device);

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

	if (m_depthPrePass)
	{
		m_depthPrePass->Shutdown();
		delete m_depthPrePass;
		m_depthPrePass = 0;
	}

	if (m_FrustumCS)
	{
		m_FrustumCS->Shutdown();
		delete m_FrustumCS;
		m_FrustumCS = 0;
	}

	if (m_CullingCS)
	{
		m_CullingCS->Shutdown();
		delete m_CullingCS;
		m_CullingCS = 0;
	}

	m_StructuredLightBuffer.Shutdown();
}

bool ForwardPlusGPU::Render(D3D * directX, Camera * camera, Model* model, LightList* lights)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;
	HRESULT hr;

	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	//m_lightList.UpdateLights(viewMatrix);

	//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
	model->Render(directX->GetDeviceContext());

	//m_depthPrePass->Render(directX->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix);

	//directX->Reset();

	//if (!m_HasGeneratedFrustum)
	//{
	//	m_FrustumCS->SetShaderParameters(directX->GetDeviceContext(), projectionMatrix);
	//	m_FrustumCS->Dispatch(directX->GetDeviceContext(), directX->GetDevice(), 16, 16, 1);
	//	m_HasGeneratedFrustum = true;
	//}

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

	/*m_depthPrePass->Bind(directX->GetDeviceContext(), 0);
	m_FrustumCS->Bind(directX->GetDeviceContext(), 1);*/
	m_CullingCS->SetShaderParameters(directX->GetDeviceContext(), projectionMatrix, viewMatrix);

	directX->GetDeviceContext()->CSSetShaderResources(2, 1, m_StructuredLightBuffer.GetResourceView());

	m_CullingCS->Dispatch(directX->GetDeviceContext(), directX->GetDevice(), 16, 16, 1);
	/*m_depthPrePass->Unbind(directX->GetDeviceContext(), 0);
	m_FrustumCS->Unbind(directX->GetDeviceContext(), 1);*/


	m_CullingCS->Bind(directX->GetDeviceContext(), 2);
	directX->GetDeviceContext()->PSSetShaderResources(1, 1, m_StructuredLightBuffer.GetResourceView());
	//Render the model using the light shader
	result = m_Shader->Render(directX->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix,
		model->GetTexture(), camera->GetPosition(), &m_lightList);
	if (!result)
	{
		return false;
	}

	m_CullingCS->Unbind(directX->GetDeviceContext(), 2);

	return true;
}

bool ForwardPlusGPU::MultiModelRender(D3D * directX, Camera * camera, Model * model, LightList * lights, Model * multiModel, XMMATRIX * worldMatrices, int numModels)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;
	HRESULT hr;

	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	//m_lightList.UpdateLights(viewMatrix);

	//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
	model->Render(directX->GetDeviceContext());

	//m_depthPrePass->Render(directX->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix);

	//directX->Reset();

	//if (!m_HasGeneratedFrustum)
	//{
	//	m_FrustumCS->SetShaderParameters(directX->GetDeviceContext(), projectionMatrix);
	//	m_FrustumCS->Dispatch(directX->GetDeviceContext(), directX->GetDevice(), 16, 16, 1);
	//	m_HasGeneratedFrustum = true;
	//}

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

	/*m_depthPrePass->Bind(directX->GetDeviceContext(), 0);
	m_FrustumCS->Bind(directX->GetDeviceContext(), 1);*/
	m_CullingCS->SetShaderParameters(directX->GetDeviceContext(), projectionMatrix, viewMatrix);

	directX->GetDeviceContext()->CSSetShaderResources(2, 1, m_StructuredLightBuffer.GetResourceView());

	m_CullingCS->Dispatch(directX->GetDeviceContext(), directX->GetDevice(), 16, 16, 1);
	/*m_depthPrePass->Unbind(directX->GetDeviceContext(), 0);
	m_FrustumCS->Unbind(directX->GetDeviceContext(), 1);*/


	m_CullingCS->Bind(directX->GetDeviceContext(), 2);
	directX->GetDeviceContext()->PSSetShaderResources(1, 1, m_StructuredLightBuffer.GetResourceView());
	//Render the model using the light shader
	result = m_Shader->Render(directX->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix,
		model->GetTexture(), camera->GetPosition(), &m_lightList);
	if (!result)
	{
		return false;
	}

	multiModel->Render(directX->GetDeviceContext());

	for (int i = 0; i < numModels; i++)
	{
		result = m_Shader->Render(directX->GetDeviceContext(), multiModel->GetIndexCount(), worldMatrices[i], viewMatrix, projectionMatrix,
			multiModel->GetTexture(), camera->GetPosition(), &m_lightList);
		if (!result)
		{
			return false;
		}
	}

	m_CullingCS->Unbind(directX->GetDeviceContext(), 2);

	return true;
}
