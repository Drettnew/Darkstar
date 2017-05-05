#pragma once
#include <d3d11.h>
#include <Windows.h>
#include <iostream>
#include <process.h>
#include <stdio.h>

#include "Camera.h"
#include "D3D.h"
#include "LightShader.h"
#include "Forward+LightShader.h"
#include "Light.h"
#include "Model.h"
#include "Light.h"

class ForwardPlusCPU
{
public:
	ForwardPlusCPU();
	~ForwardPlusCPU();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd);
	void Shutdown();

	bool Render(D3D* directX, Camera* camera, Model* model);

private:
	const static int NUM_LIGHTS = 25;
	const static int BLOCK_SIZE = 16;
	const static int SCREEN_WIDTH = 1280;
	const static int SCREEN_HEIGHT = 720;
	const static uint32_t AVERAGE_OVERLAPPING_LIGHTS_PER_TILE = 200u;

	StructuredBuffer m_StructuredLightBuffer;
	LightList m_lightList;
	ForwardPlusLightShader* m_Shader;
};