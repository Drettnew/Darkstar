#include "Model.h"

Model::Model()
{
	m_modelAsset = 0;
	m_textureAsset = 0;
	m_positionX = m_positionY = m_positionZ = 0;
	m_rotationX = m_rotationY = m_rotationZ = 0;
	m_scaleX = m_scaleY = m_scaleZ = 1;
}

Model::~Model()
{
}

bool Model::Initialize(ModelAsset * modelAsset, TextureAsset * textureAsset)
{
	//Set model and texture asset for the model
	m_modelAsset = modelAsset;
	if (!m_modelAsset)
	{
		return false;
	}

	m_textureAsset = textureAsset;
	if (!m_textureAsset)
	{
		return false;
	}

	return true;
}

void Model::Shutdown()
{
}

void Model::Render(ID3D11DeviceContext * deviceContext)
{
	m_modelAsset->Render(deviceContext);
}

int Model::GetIndexCount()
{
	return m_modelAsset->GetIndexCount();
}

void Model::SetPosition(float positionX, float positionY, float positionZ)
{
	m_positionX = positionX;
	m_positionY = positionY;
	m_positionZ = positionZ;
}

void Model::GetPosition(float & positionX, float & positionY, float & positionZ)
{
	positionX = m_positionX;
	positionY = m_positionY;
	positionZ = m_positionZ;
}

void Model::SetRotation(float rotationX, float rotationY, float rotationZ)
{
	m_rotationX = rotationX;
	m_rotationY = rotationY;
	m_rotationZ = rotationZ;
}

void Model::GetRotation(float & rotationX, float & rotationY, float & rotationZ)
{
	rotationX = m_rotationX;
	rotationY = m_rotationY;
	rotationZ = m_rotationZ;
}

void Model::SetScale(float scaleX, float scaleY, float scaleZ)
{
	m_scaleX = scaleX;
	m_scaleY = scaleY;
	m_scaleZ = scaleZ;
}

void Model::GetScale(float & scaleX, float & scaleY, float & scaleZ)
{
	scaleX = m_scaleX;
	scaleY = m_scaleY;
	scaleZ = m_scaleZ;
}

XMMATRIX Model::GetWorldMatrix()
{
	return XMMatrixScaling(m_scaleX, m_scaleY, m_scaleZ) * XMMatrixRotationRollPitchYaw(m_rotationX, m_rotationY, m_rotationZ) * XMMatrixTranslation(m_positionX, m_positionY, m_positionZ);
}

ID3D11ShaderResourceView * Model::GetTexture()
{
	return m_textureAsset->GetTexture();
}
