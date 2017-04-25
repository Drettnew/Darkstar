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
	bool LoadTexture(Assets* assets, char * filename);
public:
	Font();
	~Font();

	bool Initialize(Assets* assets, char * fontFilename, char* textureFilename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY);

private:

};