#pragma once
#include <d3d11.h>
#include <Windows.h>
#include <iostream>
#include <process.h>
#include <stdio.h>

#include "Camera.h"
#include "D3D.h"
#include "LightShader.h"
#include "Forward+CPULightShader.h"
#include "Light.h"
#include "Model.h"
#include "Light.h"

class ForwardPlusCPU
{
public:
	const static int tileX = 4;
	const static int tileY = 4;
	const static int SCREEN_WIDTH = 1280;
	const static int SCREEN_HEIGHT = 720;
	const static uint32_t AVERAGE_OVERLAPPING_LIGHTS_PER_TILE = 200u;

	ForwardPlusCPU();
	~ForwardPlusCPU();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int numLights);
	void Shutdown();

	bool Render(D3D* directX, Camera* camera, Model* model, LightList* lights);
	bool MultiModelRender(D3D* directX, Camera* camera, Model* model, LightList* lights, Model* multiModel, XMMATRIX* worldMatrices, int numModels);

private:

	StructuredBuffer m_StructuredLightBuffer;
	StructuredBuffer m_lightIndexBuffer;
	LightList m_lightList;
	ForwardPlusCPULightShader* m_Shader;

	ID3D11Texture2D* m_LightGrid;
	ID3D11ShaderResourceView* m_LightGridRV;

	bool InitializeTexture(ID3D11Device* device);
};