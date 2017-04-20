#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "Assets.h"


using namespace DirectX;
using namespace std;
class ModelAsset : public Asset
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	ModelType* m_model;

	bool InitializeBuffers(ID3D11Device* device);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadModel(const char* modelname);
public:
	ModelAsset();
	virtual ~ModelAsset();

	GRAPHIC_API bool load(std::string path, Assets* assets) override;
	GRAPHIC_API void unload() override;
	GRAPHIC_API void upload() override;

	GRAPHIC_API void Render(ID3D11DeviceContext* deviceContext);

	GRAPHIC_API int GetIndexCount();
};

