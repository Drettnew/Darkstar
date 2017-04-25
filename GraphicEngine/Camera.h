#pragma once
#include "Position.h"
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	struct CameraInputType
	{
		bool forward, back, left, right;
		int mouseX, mouseY;

		CameraInputType()
		{
			forward = back = left = right = false;
			mouseX = mouseY = -1;
		}
	};

private:
	XMMATRIX m_viewMatrix;
	XMMATRIX m_baseViewMatrix;

	Position m_cameraPosition;

public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetBaseViewToCurrent();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void HandleInputs(CameraInputType cameraInput, float dt);

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix);
	void GetBaseViewMatrix(XMMATRIX& viewMatrix);
	XMMATRIX GetBaseViewMatrix();
};

