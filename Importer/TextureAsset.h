#pragma once

#include <d3d11.h>
#include <stdio.h>
#include "Assets.h"

namespace Importer
{
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
		IMPORTER_API TextureAsset();
		IMPORTER_API virtual ~TextureAsset();

		IMPORTER_API bool load(std::string path, Assets* assets) override;
		IMPORTER_API void unload() override;
		IMPORTER_API void upload() override;

		IMPORTER_API ID3D11ShaderResourceView* GetTexture();
	private:
		bool LoadTarga(const char* filepath, int& height, int& width);

		unsigned char* m_targaData;
		ID3D11Texture2D* m_texture;
		ID3D11ShaderResourceView* m_textureView;
	};
}