#pragma once
#include <string>
#include "Util.h"
#include "D3D.h"
#include "Camera.h"
#include "ModelAsset.h"
#include "ColorShader.h"
#include "TextureShader.h"
#include "LightShader.h"
#include "Light.h"
#include "Bitmap.h"
#include "Text.h"
#include "ModelList.h"
#include "Frustum.h"

#include "ForwardRenderer.h"
#include "TextureAsset.h"
#include "Assets.h"
#include "Model.h"
#include "ObjLoader.h"
#include "ForwardPlusGPURenderer.h"
#include "ForwardPlusCPURenderer.h"
#include "DataCollector.h"

//Globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	struct GraphicInput
	{
		bool GPURender;
		bool CPURender;
		bool ForwardRender;

		bool GPURenderMulti;
		bool CPURenderMulti;
		bool ForwardRenderMulti;

		GraphicInput() {
			GPURender = CPURender = ForwardRender = false;

			GPURenderMulti = CPURenderMulti = ForwardRenderMulti = false;
		}
	};
private:
	const static int NUM_LIGHTS = 64;
	const static int NUM_MODEL = 64;

	int renderMode = 0;

	D3D* m_Direct3D;
	Camera* m_Camera;
	LightShader* m_LightShader;
	Text* m_Text;
	Frustum* m_Frustum;

	ForwardPlusGPU* m_Renderer;
	ForwardPlusCPU* m_CpuRenderer;
	ForwardRenderer* m_ForwardRender;
	//Assets* m_Assets;
	Model* m_model;
	Model* m_sphereModel;

	LightList m_lightList;

	XMMATRIX worldMatrices[NUM_MODEL];

	bool Render(float rotation);
public:
	GRAPHIC_API Graphics();
	GRAPHIC_API Graphics(const Graphics& other);
	GRAPHIC_API ~Graphics();
	GRAPHIC_API bool Initialize(int& width, int& height, HWND hwnd);
	GRAPHIC_API bool Frame(Camera::CameraInputType input, GraphicInput gInput, int fps, int cpu, float frameTime, bool logData, bool resetData);
	GRAPHIC_API void Shutdown();
};