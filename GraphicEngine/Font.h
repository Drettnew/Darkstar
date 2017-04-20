#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
using namespace std;
using namespace DirectX;

#include "TextureAsset.h"

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
	//TextureAsset* m_Texture;

	bool LoadFontData(char* filename);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename);
	void ReleaseTexture();
public:
	Font();
	~Font();

	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, char * fontFilename, char* textureFilename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY);

private:

};