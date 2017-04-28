#include "StructureBuffer.h"

StructuredBuffer::StructuredBuffer()
{
	m_buffer = 0;
	m_resourceView = 0;
	m_unorderedAccessView = 0;
}

StructuredBuffer::~StructuredBuffer()
{
}

bool StructuredBuffer::Initialize(UINT count, UINT structSize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA * data, ID3D11Device* device)
{
	HRESULT result;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.ByteWidth = count * structSize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = structSize;

	//Select the appropriate usage and CPU access flags based on the passed in flags
	if (!CPUWritable && !GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}
	else if (CPUWritable && !GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (!CPUWritable && GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE |
						 D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	else if (CPUWritable && GPUWritable)
	{
		return false;
	}

	result = device->CreateBuffer(&desc, data, &m_buffer);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void StructuredBuffer::Shutdown()
{
	if (m_buffer)
	{
		m_buffer->Release();
		delete m_buffer;
		m_buffer = 0;
	}

	if (m_resourceView)
	{
		m_resourceView->Release();
		delete m_resourceView;
		m_resourceView = 0;
	}

	if (m_unorderedAccessView)
	{
		m_unorderedAccessView->Release();
		delete m_unorderedAccessView;
		m_unorderedAccessView = 0;
	}
}

void StructuredBuffer::InitializeResourceView(ID3D11Device* device)
{
	HRESULT result;

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	m_buffer->GetDesc(&descBuf);

	//For structured buffers DXGI_FORMAT_UNKOWN must be used
	desc.Format = DXGI_FORMAT_UNKNOWN;

	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;
	desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	result = device->CreateShaderResourceView(m_buffer, &desc, &m_resourceView);
}

ID3D11ShaderResourceView * StructuredBuffer::GetResourceView()
{
	return m_resourceView;
}
