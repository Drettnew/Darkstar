#pragma once

#include <d3d11.h>
#include <stdio.h>
#include "Assets.h"

	class TextureAsset : public Asset
	{
	private:
		struct TargaHeader
		{
			unsigned char data1[12];
			unsigned short width;
			unsigned short height;
			unsigned char bpp;
			unsigned char data2;
		};
	public:
		GRAPHIC_API TextureAsset();
		GRAPHIC_API virtual ~TextureAsset();

		GRAPHIC_API bool load(std::string path, Assets* assets) override;
		GRAPHIC_API void unload() override;
		GRAPHIC_API void upload() override;

		GRAPHIC_API ID3D11ShaderResourceView* GetTexture();
	private:
		bool LoadTarga(const char* filepath, int& height, int& width);

		unsigned char* m_targaData;
		ID3D11Texture2D* m_texture;
		ID3D11ShaderResourceView* m_textureView;
	};
