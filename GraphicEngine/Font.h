#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
using namespace std;
using namespace DirectX;

#include "TextureAsset.h"
#include "Assets.h"

class Font
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	FontType* m_Font;
	TextureAsset* m_Texture;

	bool LoadFontData(char* filename);
	void ReleaseFontData();
	bool LoadTexture(char * filename, ID3D11Device * device, ID3D11DeviceContext * deviceContext);
public:
	Font();
	~Font();

	bool Initialize(char * fontFilename, char* textureFilename, ID3D11Device * device, ID3D11DeviceContext * deviceContext);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY);

private:

};