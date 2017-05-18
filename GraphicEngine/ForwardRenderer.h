#pragma once
#include <d3d11.h>

#include "Camera.h"
#include "D3D.h"
#include "ModelAsset.h"
#include "Light.h"
#include "ModelList.h"
#include "LightShader.h"

class ForwardRenderer
{
public:
	ForwardRenderer();
	~ForwardRenderer();

	bool Initialize(ID3D11Device* device, HWND hwnd, int numLights);
	void Shutdown();
	bool Render(D3D* directX, Camera* camera, Model* model, LightList* lights);

	bool MultiModelRender(D3D* directX, Camera* camera, Model* model, LightList* lights, Model* multiModel, XMMATRIX* worldMatrices, int numModels);

private:
	LightShader* m_Shader;
	StructuredBuffer m_StructuredLightBuffer;
};