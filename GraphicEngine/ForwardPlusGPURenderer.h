#pragma once
#include <d3d11.h>

#include "Camera.h"
#include "D3D.h"
#include "LightShader.h"
#include "Forward+LightShader.h"
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

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int numLights);
	void Shutdown();

	bool Render(D3D* directX, Camera* camera, Model* model, LightList* lights);
	bool MultiModelRender(D3D* directX, Camera* camera, Model* model, LightList* lights, Model* multiModel, XMMATRIX* worldMatrices, int numModels);

private:
	const static int NUM_LIGHTS = 2048;
	StructuredBuffer m_StructuredLightBuffer;

	LightList m_lightList;

	ForwardPlusLightShader* m_Shader;
	FrustumComputeShader* m_FrustumCS;
	DepthPass* m_depthPrePass;

	CullingComputeShader* m_CullingCS;

	bool m_HasGeneratedFrustum = false;
};