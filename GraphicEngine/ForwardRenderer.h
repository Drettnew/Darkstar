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

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();

	bool SetupData(ModelList* modelList, Light* light, ModelAsset* model);
	bool Render(D3D* directX, Camera* camera);

private:
	ModelList* m_frameModels;
	Light* m_frameLights;

	//TEMP
	ModelAsset* m_Model;
	LightShader* m_Shader;
};