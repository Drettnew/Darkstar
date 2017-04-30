#pragma once
#include <d3d11.h>

#include "Camera.h"
#include "D3D.h"
#include "LightShader.h"
#include "Light.h"
#include "Model.h"
#include "DepthPass.h"
#include "FrustumComputeShader.h"

class ForwardPlusGPU
{
public:
	ForwardPlusGPU();
	~ForwardPlusGPU();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd);
	void Shutdown();

	bool Render(D3D* directX, Camera* camera, Model* model);

private:
	LightList m_lightList;

	LightShader* m_Shader;
	FrustumComputeShader* m_frustumCS;
	DepthPass* m_depthPrePass;
};