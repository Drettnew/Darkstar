#include "ForwardPlusCPURenderer.h"

unsigned int GlobalIndexCounter = 0;
HANDLE Mutex;
uint32_t lightIndexList[ForwardPlusCPU::tileX * ForwardPlusCPU::tileY * ForwardPlusCPU::AVERAGE_OVERLAPPING_LIGHTS_PER_TILE];
UINT32 lightGrid[ForwardPlusCPU::tileX * ForwardPlusCPU::tileY * 2];

ForwardPlusCPU::ForwardPlusCPU()
{
	m_Shader = 0;
}

ForwardPlusCPU::~ForwardPlusCPU()
{
}

bool ForwardPlusCPU::Initialize(ID3D11Device * device, ID3D11DeviceContext* deviceContext, HWND hwnd, int numLights)
{
	bool result;

	// Create the light shader object.
	m_Shader = new ForwardPlusCPULightShader;
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

	//m_lightList.Random(50, 50, numLights);

	m_StructuredLightBuffer.Initialize(numLights, sizeof(Light), true, false, NULL, device);
	m_StructuredLightBuffer.InitializeResourceView(device);

	m_lightIndexBuffer.Initialize(tileX * tileY * AVERAGE_OVERLAPPING_LIGHTS_PER_TILE, sizeof(uint32_t), true, false, NULL, device);
	m_lightIndexBuffer.InitializeResourceView(device);

	InitializeTexture(device);

	return true;
}

void ForwardPlusCPU::Shutdown()
{
	// Release the color shader object.
	if (m_Shader)
	{
		m_Shader->Shutdown();
		delete m_Shader;
		m_Shader = 0;
	}

	if (m_LightGrid)
	{
		m_LightGrid->Release();
		m_LightGrid = 0;
	}

	if (m_LightGridRV)
	{
		m_LightGridRV->Release();
		m_LightGridRV = 0;
	}

	m_StructuredLightBuffer.Shutdown();
	m_lightIndexBuffer.Shutdown();
}

XMFLOAT4 ClipToView(XMFLOAT4 clip, XMMATRIX invProjectionMatrix)
{
	XMFLOAT4 view;
	XMStoreFloat4(&view, XMVector3Transform(XMLoadFloat4(&clip), invProjectionMatrix));

	view.x = view.x / view.w;
	view.y = view.y / view.w;
	view.z = view.z / view.w;

	return view;
}

XMFLOAT3 CreatePlaneEquation(XMFLOAT4 p0, XMFLOAT4 p1)
{
	XMVECTOR b = XMLoadFloat4(&p0);
	XMVECTOR c = XMLoadFloat4(&p1);

	XMVECTOR cross = XMVector3Cross(b, c);

	cross = XMVector3Normalize(cross);

	XMFLOAT3 result;

	XMStoreFloat3(&result, cross);
	
	return result;
}

float GetSignedDistanceFromPlane(XMFLOAT4 p, XMFLOAT3 eqn)
{
	float result = eqn.x * p.x + eqn.y * p.y + eqn.z * p.z;

	return result;
}

struct parameter
{
	XMMATRIX invProj;
	XMFLOAT2 threadID;
	std::vector<Light> lightList;
};

unsigned int _stdcall myThread(LPVOID lpParameter)
{
	unsigned int lightList[1024];
	unsigned int lightCount = 0;

	parameter* param = (parameter*)lpParameter;

	const int tileX = 4;
	const int tileY = 4;

	const int screen_width = 1280;
	const int screen_height = 720;

	int blockSizeX = screen_width / tileX;
	int blockSizeY = screen_height / tileY;

	unsigned int pxm = blockSizeX * param->threadID.x;
	unsigned int pym = blockSizeY * param->threadID.y;
	unsigned int pxp = blockSizeX * (param->threadID.x + 1);
	unsigned int pyp = blockSizeY * (param->threadID.y + 1);

	XMFLOAT4 p[4];
	XMFLOAT3 frustum[4];

	p[0] = ClipToView(XMFLOAT4((float)pxm / (float)screen_width * 2.0f - 1.0f, ((float)screen_height - (float)pym) / (float)screen_height * 2.0f - 1.0f, 1.0f, 1.0f), param->invProj);
	p[1] = ClipToView(XMFLOAT4((float)pxp / (float)screen_width * 2.0f - 1.0f, ((float)screen_height - (float)pym) / (float)screen_height * 2.0f - 1.0f, 1.0f, 1.0f), param->invProj);
	p[2] = ClipToView(XMFLOAT4((float)pxp / (float)screen_width * 2.0f - 1.0f, ((float)screen_height - (float)pyp) / (float)screen_height * 2.0f - 1.0f, 1.0f, 1.0f), param->invProj);
	p[3] = ClipToView(XMFLOAT4((float)pxm / (float)screen_width * 2.0f - 1.0f, ((float)screen_height - (float)pyp) / (float)screen_height * 2.0f - 1.0f, 1.0f, 1.0f), param->invProj);

	frustum[0] = CreatePlaneEquation(p[0], p[1]);
	frustum[1] = CreatePlaneEquation(p[1], p[2]);
	frustum[2] = CreatePlaneEquation(p[2], p[3]);
	frustum[3] = CreatePlaneEquation(p[3], p[0]);

	for (int i = 0; i < param->lightList.size(); i++)
	{
		Light light = param->lightList[i];

		float r = light.m_Range;
		XMFLOAT4 center = light.m_PositionVS;

		bool frustum0 = GetSignedDistanceFromPlane(center, frustum[0]) < r;
		bool frustum1 = GetSignedDistanceFromPlane(center, frustum[1]) < r;
		bool frustum2 = GetSignedDistanceFromPlane(center, frustum[2]) < r;
		bool frustum3 = GetSignedDistanceFromPlane(center, frustum[3]) < r;

		if (frustum0 &&
			frustum1 &&
			frustum2 &&
			frustum3)
		{
			if (lightCount < 1024)
			{
				lightList[lightCount] = i;

				lightCount++;
			}
		}
	}

	if (WaitForSingleObject(Mutex, INFINITE) == WAIT_OBJECT_0)
	{
		int startOffset = GlobalIndexCounter;

		GlobalIndexCounter = GlobalIndexCounter + lightCount;

		lightGrid[((int)param->threadID.y * tileX * 2) + ((int)param->threadID.x * 2)] = startOffset;
		lightGrid[((int)param->threadID.y * tileX * 2) + ((int)param->threadID.x * 2) + 1] = lightCount;

		for (int i = 0; i < lightCount; i++)
		{
			lightIndexList[startOffset + i] = lightList[i];
		}

		ReleaseMutex(Mutex);
	}

	return 0;
}

bool ForwardPlusCPU::Render(D3D * directX, Camera * camera, Model* model, LightList* lights)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;
	HRESULT hr;

	GlobalIndexCounter = 0;

	parameter dataArray[tileX * tileY];

	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	//m_lightList.UpdateLights(viewMatrix);

	XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projectionMatrix);

	Mutex = CreateMutex(NULL, FALSE, NULL);

	HANDLE threadHandles[tileX * tileY];
	for (int y = 0; y < tileY; y++)
	{
		for (int x = 0; x < tileX; x++)
		{

			dataArray[(y * tileX) + x].invProj = invProjMatrix;
			dataArray[(y * tileX) + x].lightList = lights->GetLightList();
			dataArray[(y * tileX) + x].threadID.x = x;
			dataArray[(y * tileX) + x].threadID.y = y;

			threadHandles[(y * tileX) + x] = (HANDLE)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)myThread, &dataArray[(y * tileX) + x], 0, NULL);
		}
	}
	WaitForMultipleObjects(tileX * tileY, threadHandles, true, INFINITE);
	for (int i = 0; i < tileX * tileY; i++)
	{
		CloseHandle(threadHandles[i]);
	}

	CloseHandle(Mutex);

	// Set the row pitch of the targa image data.
	int rowPitch = (tileX * 2) * sizeof(UINT32);

	//Copy the tarha image data into the texture
	directX->GetDeviceContext()->UpdateSubresource(m_LightGrid, 0, NULL, lightGrid, rowPitch, 0);

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

	std::vector<Light> lightsVector = lights->GetLightList();
	XMVECTOR xmvector;
	XMMATRIX transposedViewMatrix;

	transposedViewMatrix = viewMatrix;
	//transposedViewMatrix = XMMatrixTranspose(viewMatrix);

	for (int i = 0; i < lightsVector.size(); i++)
	{
		Light light = lightsVector[i];

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

	//Lock the light constant buffer so it can be written to
	hr = directX->GetDeviceContext()->Map(m_lightIndexBuffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	memcpy(mappedResource.pData, lightIndexList, sizeof(lightIndexList));

	//Unlock the constant buffer
	directX->GetDeviceContext()->Unmap(m_lightIndexBuffer.GetBuffer(), 0);

	directX->GetDeviceContext()->PSSetShaderResources(3, 1, &m_LightGridRV);
	directX->GetDeviceContext()->PSSetShaderResources(2, 1, m_lightIndexBuffer.GetResourceView());
	directX->GetDeviceContext()->PSSetShaderResources(1, 1, m_StructuredLightBuffer.GetResourceView());

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

bool ForwardPlusCPU::MultiModelRender(D3D * directX, Camera * camera, Model * model, LightList * lights, Model * multiModel, XMMATRIX * worldMatrices, int numModels)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;
	HRESULT hr;

	GlobalIndexCounter = 0;

	parameter dataArray[tileX * tileY];

	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	//m_lightList.UpdateLights(viewMatrix);

	XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projectionMatrix);

	Mutex = CreateMutex(NULL, FALSE, NULL);

	HANDLE threadHandles[tileX * tileY];
	for (int y = 0; y < tileY; y++)
	{
		for (int x = 0; x < tileX; x++)
		{

			dataArray[(y * tileX) + x].invProj = invProjMatrix;
			dataArray[(y * tileX) + x].lightList = lights->GetLightList();
			dataArray[(y * tileX) + x].threadID.x = x;
			dataArray[(y * tileX) + x].threadID.y = y;

			threadHandles[(y * tileX) + x] = (HANDLE)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)myThread, &dataArray[(y * tileX) + x], 0, NULL);
		}
	}
	WaitForMultipleObjects(tileX * tileY, threadHandles, true, INFINITE);
	for (int i = 0; i < tileX * tileY; i++)
	{
		CloseHandle(threadHandles[i]);
	}

	CloseHandle(Mutex);

	// Set the row pitch of the targa image data.
	int rowPitch = (tileX * 2) * sizeof(UINT32);

	//Copy the tarha image data into the texture
	directX->GetDeviceContext()->UpdateSubresource(m_LightGrid, 0, NULL, lightGrid, rowPitch, 0);

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

	std::vector<Light> lightsVector = lights->GetLightList();
	XMVECTOR xmvector;
	XMMATRIX transposedViewMatrix;

	transposedViewMatrix = viewMatrix;
	//transposedViewMatrix = XMMatrixTranspose(viewMatrix);

	for (int i = 0; i < lightsVector.size(); i++)
	{
		Light light = lightsVector[i];

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

	//Lock the light constant buffer so it can be written to
	hr = directX->GetDeviceContext()->Map(m_lightIndexBuffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	memcpy(mappedResource.pData, lightIndexList, sizeof(lightIndexList));

	//Unlock the constant buffer
	directX->GetDeviceContext()->Unmap(m_lightIndexBuffer.GetBuffer(), 0);

	directX->GetDeviceContext()->PSSetShaderResources(3, 1, &m_LightGridRV);
	directX->GetDeviceContext()->PSSetShaderResources(2, 1, m_lightIndexBuffer.GetResourceView());
	directX->GetDeviceContext()->PSSetShaderResources(1, 1, m_StructuredLightBuffer.GetResourceView());

	//Put the model vertex and index buffer on the graphics pipeline to prepare them for drawing
	model->Render(directX->GetDeviceContext());

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

	return true;
}

bool ForwardPlusCPU::InitializeTexture(ID3D11Device * device)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC textureDesc;

	DXGI_SAMPLE_DESC sampleDesc;

	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = tileX;
	textureDesc.Height = tileY;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32_UINT;
	textureDesc.MipLevels = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc = sampleDesc;

	result = device->CreateTexture2D(&textureDesc, nullptr, &m_LightGrid);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	resourceViewDesc.Format = DXGI_FORMAT_R32G32_UINT;
	resourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MipLevels = 1;
	resourceViewDesc.Texture2D.MostDetailedMip = 0;

	result = device->CreateShaderResourceView(m_LightGrid, &resourceViewDesc, &m_LightGridRV);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}
