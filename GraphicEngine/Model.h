#pragma once

#include <d3d11.h>

#include "ModelAsset.h"
#include "TextureAsset.h"

class Model
{
private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_scaleX, m_scaleY, m_scaleZ;

	ModelAsset* m_modelAsset;
	TextureAsset* m_textureAsset;
public:
	Model();
	~Model();

	bool Initialize(ModelAsset* modelAsset, TextureAsset* textureAsset);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

	void SetPosition(float positionX, float positionY, float positionZ);
	void GetPosition(float& positionX, float& positionY, float& positionZ);

	void SetRotation(float rotationX, float rotationY, float rotationZ);
	void GetRotation(float& rotationX, float& rotationY, float& rotationZ);

	void SetScale(float scaleX, float scaleY, float scaleZ);
	void GetScale(float& scaleX, float& scaleY, float& scaleZ);

	XMMATRIX GetWorldMatrix();
	ID3D11ShaderResourceView* GetTexture();
private:

};