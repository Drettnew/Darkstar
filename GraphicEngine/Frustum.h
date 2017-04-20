#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Frustum
{
public:
	Frustum();
	~Frustum();

	void Initialize(float screenDepth);

	void ConstructFrustum(XMMATRIX projectionMatrix, XMMATRIX viewMatrix);

	bool CheckPoint(float x, float y, float z);
	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);

private:
	float m_screenDepth;
	float m_planes[6][4];
};