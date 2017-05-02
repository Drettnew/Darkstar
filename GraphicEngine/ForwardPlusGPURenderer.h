#pragma once
#include <d3d11.h>

#include "Camera.h"
#include "D3D.h"
#include "LightShader.h"
#include "Light.h"
#include "Model.h"
#include "DepthPass.h"
#include "FrustumComputeShader.h"
#include "CullingComputeShader.h"
#include "Light.h"

class ForwardPlusGPU
{
public:
	ForwardPlusGPU();
	~ForwardPlusGPU();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd);
	void Shutdown();

	bool Render(D3D* directX, Camera* camera, Model* model);

private:
	const static int NUM_LIGHTS = 1;
	StructuredBuffer m_StructuredLightBuffer;

	LightList m_lightList;

	LightShader* m_Shader;
	FrustumComputeShader* m_FrustumCS;
	DepthPass* m_depthPrePass;

	CullingComputeShader* m_CullingCS;

	bool m_HasGeneratedFrustum = false;
};