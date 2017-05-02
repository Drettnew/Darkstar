#include "CullingComputeShader.h"

CullingComputeShader::CullingComputeShader()
{
	m_computeShader = 0;
	m_ScreenToViewParamBuffer = 0;
	m_dispatchBuffer = 0;
	m_LightGrid = 0;
	m_LightGridRV = 0;
	m_LightGridUAV = 0;
}

CullingComputeShader::~CullingComputeShader()
{
}

bool CullingComputeShader::Initialize(ID3D11Device * device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../GraphicEngine/Shaders/culling_cs.hlsl");
	if (!result)
	{
		return false;
	}

	D3D11_BUFFER_DESC dispatchBufferDesc;
	D3D11_BUFFER_DESC ScreenToViewBufferDesc;

	m_dispatchParameters.numThreadGroups = XMUINT3(std::ceil(SCREEN_WIDTH / (float)BLOCK_SIZE), std::ceil(SCREEN_HEIGHT / (float)BLOCK_SIZE), 1);
	m_dispatchParameters.numThreads = XMUINT3(std::ceil(m_dispatchParameters.numThreadGroups.x * (float)BLOCK_SIZE), std::ceil(m_dispatchParameters.numThreadGroups.y * (float)BLOCK_SIZE), 1);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	dispatchBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	dispatchBufferDesc.ByteWidth = sizeof(DispatchParams);
	dispatchBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dispatchBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dispatchBufferDesc.MiscFlags = 0;
	dispatchBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&dispatchBufferDesc, NULL, &m_dispatchBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	ScreenToViewBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ScreenToViewBufferDesc.ByteWidth = sizeof(ScreenToViewParams);
	ScreenToViewBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ScreenToViewBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ScreenToViewBufferDesc.MiscFlags = 0;
	ScreenToViewBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&ScreenToViewBufferDesc, NULL, &m_ScreenToViewParamBuffer);
	if (FAILED(result))
	{
		return false;
	}

	CreateStructuredBuffer(device);

	return true;
}

bool CullingComputeShader::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DispatchParams* dataPtr;
	ScreenToViewParams* dataPtr2;

	//Lock the light constant buffer so it can be written to
	result = deviceContext->Map(m_dispatchBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	dataPtr = (DispatchParams*)mappedResource.pData;

	dataPtr->numThreadGroups = m_dispatchParameters.numThreadGroups;
	dataPtr->numThreads = m_dispatchParameters.numThreads;

	//Unlock the constant buffer
	deviceContext->Unmap(m_dispatchBuffer, 0);

	deviceContext->CSSetConstantBuffers(0, 1, &m_dispatchBuffer);

	//Lock the light constant buffer so it can be written to
	result = deviceContext->Map(m_ScreenToViewParamBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (ScreenToViewParams*)mappedResource.pData;

	XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projectionMatrix);
	invProjMatrix = XMMatrixTranspose(invProjMatrix);

	dataPtr2->InverseProjectionMatrix = invProjMatrix;
	dataPtr2->ScreenDimensions = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);

	//Unlock the constant buffer
	deviceContext->Unmap(m_ScreenToViewParamBuffer, 0);

	deviceContext->CSSetConstantBuffers(1, 1, &m_ScreenToViewParamBuffer);

	return true;
}

void CullingComputeShader::Shutdown()
{
	ShutdownShader();

	m_LightIndexCounter.Shutdown();
	m_LightIndexList.Shutdown();

	if (m_dispatchBuffer)
	{
		m_dispatchBuffer->Release();
		m_dispatchBuffer = 0;
	}

	if (m_computeShader)
	{
		m_computeShader->Release();
		m_computeShader = 0;
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

	if (m_LightGridUAV)
	{
		m_LightGridUAV->Release();
		m_LightGridUAV = 0;
	}
}

void CullingComputeShader::Dispatch(ID3D11DeviceContext * deviceContext, ID3D11Device * device, int x, int y, int z)
{
	m_LightIndexCounter.Copy(deviceContext, &m_indexCounterInitialBuffer);

	deviceContext->CSSetShader(m_computeShader, NULL, 0);

	deviceContext->CSSetUnorderedAccessViews(0, 1, m_LightIndexCounter.GetUnorderedAccessView(), 0);
	deviceContext->CSSetUnorderedAccessViews(1, 1, m_LightIndexList.GetUnorderedAccessView(), 0);
	deviceContext->CSSetUnorderedAccessViews(2, 1, &m_LightGridUAV, 0);

	deviceContext->Dispatch(m_dispatchParameters.numThreadGroups.x, m_dispatchParameters.numThreadGroups.y, m_dispatchParameters.numThreadGroups.z);
}

void CullingComputeShader::CreateStructuredBuffer(ID3D11Device * device)
{

	D3D11_SUBRESOURCE_DATA subResourceData;

	uint32_t initialValue = 0;

	m_indexCounterInitialBuffer.Initialize(1, sizeof(uint32_t), false, true, &initialValue, device);

	m_LightIndexCounter.Initialize(1, sizeof(uint32_t), false, true, &initialValue, device);
	m_LightIndexCounter.InitializeAccessView(device);

	m_LightIndexList.Initialize(m_dispatchParameters.numThreadGroups.x * m_dispatchParameters.numThreadGroups.y * m_dispatchParameters.numThreadGroups.z * AVERAGE_OVERLAPPING_LIGHTS_PER_TILE, sizeof(uint32_t), false, true, NULL, device);
	m_LightIndexList.InitializeAccessView(device);

	InitializeTexture(device);
}

void CullingComputeShader::DestroyStructuredBuffer()
{
	m_LightIndexCounter.Shutdown();
	m_LightIndexList.Shutdown();

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

	if (m_LightGridUAV)
	{
		m_LightGridUAV->Release();
		m_LightGridUAV = 0;
	}
}

bool CullingComputeShader::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* computeShader;

	errorMessage = 0;
	computeShader = 0;

	result = D3DCompileFromFile(filename, NULL, NULL, "CullingComputeShader", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&computeShader, &errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, filename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = device->CreateComputeShader(computeShader->GetBufferPointer(), computeShader->GetBufferSize(), NULL, &m_computeShader);
	if (FAILED(result))
	{
		return false;
	}

	computeShader->Release();
	computeShader = 0;

	return true;
}

bool CullingComputeShader::InitializeTexture(ID3D11Device * device)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC textureDesc;

	DXGI_SAMPLE_DESC sampleDesc;

	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = m_dispatchParameters.numThreadGroups.x;
	textureDesc.Height = m_dispatchParameters.numThreadGroups.y;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32_UINT;
	textureDesc.MipLevels = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
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

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
	unorderedAccessViewDesc.Format = DXGI_FORMAT_R32G32_UINT;
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	unorderedAccessViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateUnorderedAccessView(m_LightGrid, &unorderedAccessViewDesc, &m_LightGridUAV);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void CullingComputeShader::ShutdownShader()
{
	if (m_computeShader)
	{
		m_computeShader->Release();
		m_computeShader = 0;
	}

	if (m_dispatchBuffer)
	{
		m_dispatchBuffer->Release();
		m_dispatchBuffer = 0;
	}

	if (m_ScreenToViewParamBuffer)
	{
		m_ScreenToViewParamBuffer->Release();
		m_ScreenToViewParamBuffer = 0;
	}
}

void CullingComputeShader::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}
