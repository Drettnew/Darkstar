#pragma once
#include <d3d11.h>

class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

	bool Initialize(UINT count, UINT structSize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* data, ID3D11Device* device);
	void Shutdown();

	void InitializeResourceView(ID3D11Device* device);
	void InitializeAccessView(ID3D11Device* device);

	ID3D11ShaderResourceView* GetResourceView();
	ID3D11Buffer* GetBuffer();
	ID3D11UnorderedAccessView* GetUnorderedAccessView();

private:
	ID3D11Buffer* m_buffer;
	ID3D11ShaderResourceView* m_resourceView;
	ID3D11UnorderedAccessView* m_unorderedAccessView;
};