#pragma once
#include <d3d11.h>

#include "Camera.h"
#include "D3D.h"
#include "LightShader.h"
#include "Light.h"
#include "Model.h"

class ForwardPlusGPU
{
public:
	ForwardPlusGPU();
	~ForwardPlusGPU();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();

	bool Render(D3D* directX, Camera* camera, Model* model);

private:
	LightList m_lightList;
	LightShader* m_Shader;
};