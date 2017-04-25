#include "Camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::SetBaseViewToCurrent()
{
	m_baseViewMatrix = m_viewMatrix;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_cameraPosition.SetPosition(x, y, z);
}

void Camera::SetRotation(float x, float y, float z)
{
	m_cameraPosition.SetRotation(x, y, z);
}

XMFLOAT3 Camera::GetPosition()
{
	float positionX, positionY, positionZ;

	//Get the position
	m_cameraPosition.GetPosition(positionX, positionY, positionZ);

	return XMFLOAT3(positionX, positionY, positionZ);
}

XMFLOAT3 Camera::GetRotation()
{
	float rotationX, rotationY, rotationZ;

	//Get the camera rotation
	m_cameraPosition.GetRotation(rotationX, rotationY, rotationZ);

	return XMFLOAT3(rotationX, rotationY, rotationZ);
}

void Camera::HandleInputs(CameraInputType cameraInput, float dt)
{
	//Set the frame time used for updating the postion
	m_cameraPosition.SetFrameTime(dt);

	if (cameraInput.forward || false)
	{
		m_cameraPosition.MoveForward(cameraInput.forward);
	}
	//Update the position
	m_cameraPosition.MoveForward(cameraInput.forward);
	m_cameraPosition.MoveBackward(cameraInput.back);

	//Update the rotation
	m_cameraPosition.TurnLeft(cameraInput.left);
	m_cameraPosition.TurnRight(cameraInput.right);
}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//setup the vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	m_cameraPosition.GetPosition(position.x, position.y, position.z);


	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	//Get the camera rotation
	m_cameraPosition.GetRotation(pitch, yaw, roll);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = pitch * 0.0174532925f;
	yaw = yaw * 0.0174532925f;
	roll = roll * 0.0174532925f;

	//Create the rotation matrix form the yaw, pitch and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//Translate the rotated camera postion to the location of the viewer
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//Finally create the view matrix from the three updated vectors
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Camera::GetViewMatrix(XMMATRIX &viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void Camera::GetBaseViewMatrix(XMMATRIX & viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
}

XMMATRIX Camera::GetBaseViewMatrix()
{
	return m_baseViewMatrix;
}
