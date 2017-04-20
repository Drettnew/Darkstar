#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_baseViewMatrix;

public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetBaseViewToCurrent();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix);
	void GetBaseViewMatrix(XMMATRIX& viewMatrix);
	XMMATRIX GetBaseViewMatrix();
};

