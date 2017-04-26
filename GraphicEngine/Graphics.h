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

//Globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	LightShader* m_LightShader;
	Text* m_Text;
	ModelList* m_ModelList;
	Frustum* m_Frustum;

	ForwardRenderer* m_Renderer;
	Assets* m_Assets;
	Model model;
	ModelList modelList;

	bool Render(float rotation);
public:
	GRAPHIC_API Graphics();
	GRAPHIC_API Graphics(const Graphics& other);
	GRAPHIC_API ~Graphics();
	GRAPHIC_API bool Initialize(int& width, int& height, HWND hwnd);
	GRAPHIC_API bool Frame(Camera::CameraInputType input, int fps, int cpu, float frameTime);
	GRAPHIC_API void Shutdown();
};