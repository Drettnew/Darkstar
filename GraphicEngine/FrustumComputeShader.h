#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "StructureBuffer.h"

using namespace DirectX;
using namespace std;

class FrustumComputeShader
{
	struct Frustum
	{
		XMFLOAT4 planes[4];
	};

	// Constant buffer to store the number of groups executed in a dispatch.
	__declspec(align(16)) struct DispatchParams
	{
		XMUINT3 numThreadGroups;
		uint32_t padding0;        // Pad to 16 bytes.
		XMUINT3 numThreads;
		uint32_t padding1;        // Pad to 16 bytes.
	};

	__declspec(align(16)) struct ScreenToViewParams
	{
		XMMATRIX InverseProjectionMatrix;
		XMFLOAT2 ScreenDimensions;
		XMFLOAT2 Padding;
	};
public:
	FrustumComputeShader();
	~FrustumComputeShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX projectionMatrix);
	void Shutdown();

	void Bind(ID3D11DeviceContext* deviceContext, UINT startSlot);
	void Unbind(ID3D11DeviceContext* deviceContext, UINT startSlot);

	void Dispatch(ID3D11DeviceContext* deviceContext, ID3D11Device * device, int x, int y, int z);
private:
	const static int BLOCK_SIZE = 16;
	const static int SCREEN_WIDTH = 800;
	const static int SCREEN_HEIGHT = 480;

	ID3D11ComputeShader* m_computeShader;
	StructuredBuffer m_structuredBuffer;
	ID3D11Buffer* m_dispatchBuffer;
	ID3D11Buffer* m_StoVParamBuffer;

	DispatchParams m_dispatchParameters;

	void CreateStructuredBuffer(ID3D11Device * device);
	void DestroyStructuredBuffer();

	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
};
