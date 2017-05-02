#pragma once
#include <d3d11.h>
#include <vector>

class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

	bool Initialize(UINT count, UINT structSize, bool CPUWritable, bool GPUWritable, const void* data, ID3D11Device* device);
	void Shutdown();

	void InitializeResourceView(ID3D11Device* device);
	void InitializeAccessView(ID3D11Device* device);
	void Copy(ID3D11DeviceContext* deviceContext, StructuredBuffer* structuredBuffer);

	ID3D11ShaderResourceView* const * GetResourceView();
	ID3D11Buffer* GetBuffer();
	ID3D11UnorderedAccessView* const * GetUnorderedAccessView();

private:
	ID3D11Buffer* m_buffer;
	ID3D11ShaderResourceView* m_resourceView;
	ID3D11UnorderedAccessView* m_unorderedAccessView;
};