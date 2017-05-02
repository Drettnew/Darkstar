#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "StructureBuffer.h"
using namespace DirectX;
using namespace std;

class CullingComputeShader
{
	struct DispatchParams
	{
		XMUINT3 numThreadGroups;
		XMUINT3 numThreads;
		XMUINT2 padding;
	};

	struct ScreenToViewParams
	{
		XMMATRIX InverseProjectionMatrix;
		XMFLOAT2 ScreenDimensions;
	};
public:
	CullingComputeShader();
	~CullingComputeShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX projectionMatrix);
	void Shutdown();

	void Dispatch(ID3D11DeviceContext* deviceContext, ID3D11Device * device, int x, int y, int z);

private:
	const static int BLOCK_SIZE = 16;
	const static int SCREEN_WIDTH = 1280;
	const static int SCREEN_HEIGHT = 720;
	const static uint32_t AVERAGE_OVERLAPPING_LIGHTS_PER_TILE = 200u;

	StructuredBuffer m_indexCounterInitialBuffer;

	StructuredBuffer m_LightIndexCounter;
	StructuredBuffer m_LightIndexList;

	ID3D11ComputeShader* m_computeShader;
	ID3D11Buffer* m_dispatchBuffer;
	ID3D11Buffer* m_ScreenToViewParamBuffer;

	DispatchParams m_dispatchParameters;

	ID3D11Texture2D* m_LightGrid;

	ID3D11UnorderedAccessView* m_LightGridUAV;
	ID3D11ShaderResourceView* m_LightGridRV;

	void CreateStructuredBuffer(ID3D11Device * device);
	void DestroyStructuredBuffer();

	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);
	bool InitializeTexture(ID3D11Device* device);

	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
};