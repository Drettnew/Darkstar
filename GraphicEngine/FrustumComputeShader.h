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

	struct DispatchParams
	{
		XMUINT3 numThreadGroups;
		XMUINT3 numThreads;
		XMUINT2 padding;
	};
public:
	FrustumComputeShader();
	~FrustumComputeShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX projectionMatrix);
	void Shutdown();

	void Dispatch(ID3D11DeviceContext* deviceContext, int x, int y, int z);
private:
	const static int BLOCK_SIZE = 16;
	const static int SCREEN_WIDTH = 800;
	const static int SCREEN_HEIGHT = 600;

	ID3D11ComputeShader* m_computeShader;
	StructuredBuffer m_structuredBuffer;
	ID3D11Buffer* m_dispatchBuffer;
	ID3D11Buffer* m_StoVParamBuffer;

	DispatchParams m_dispatchParameters;

	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
};
