#include "Position.h"

Position::Position()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_frameTime = 0.0f;

	m_forwardSpeed = 0.0f;
	m_backwardSpeed = 0.0f;
	m_upwardSpeed = 0.0f;
	m_downwardSpeed = 0.0f;
	m_leftTurnSpeed = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed = 0.0f;
	m_lookDownSpeed = 0.0f;
}

Position::~Position()
{
}

void Position::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void Position::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

void Position::GetPosition(float & x, float & y, float & z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
}

void Position::GetRotation(float & x, float & y, float & z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
}

void Position::SetFrameTime(float frameTime)
{
	m_frameTime = frameTime;
}

void Position::MoveForward(bool keydown)
{
	float radians, radiansX;

	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_forwardSpeed += m_frameTime * 0.001f;
		if (m_forwardSpeed > (m_frameTime * 0.005f))
		{
			m_forwardSpeed = m_frameTime * 0.005f;
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime * 0.0005f;

		if (m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotationY * 0.0174532925f;
	radiansX = m_rotationX * 0.0174532925f;

	// Update the position.
	m_positionX += sinf(radians) * m_forwardSpeed;
	m_positionZ += cosf(radians) * m_forwardSpeed;
	m_positionY -= sinf(radiansX) * m_forwardSpeed;

	
}

void Position::MoveBackward(bool keydown)
{
	float radians, radiansX;

	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_backwardSpeed += m_frameTime * 0.001f;

		if (m_backwardSpeed > (m_frameTime * 0.005f))
		{
			m_backwardSpeed = m_frameTime * 0.005f;
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * 0.0005f;

		if (m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotationY * 0.0174532925f;
	radiansX = m_rotationX * 0.0174532925f;

	// Update the position.
	m_positionX -= sinf(radians) * m_backwardSpeed;
	m_positionZ -= cosf(radians) * m_backwardSpeed;
	m_positionY += sinf(radiansX) * m_backwardSpeed;

}

void Position::MoveUpward(bool keydown)
{
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_upwardSpeed += m_frameTime * 0.01f;

		if (m_upwardSpeed > (m_frameTime * 0.15f))
		{
			m_upwardSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * 0.005f;

		if (m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_positionY += m_upwardSpeed;
}

void Position::MoveDownward(bool keydown)
{
	// Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_downwardSpeed += m_frameTime * 0.01f;

		if (m_downwardSpeed > (m_frameTime * 0.15f))
		{
			m_downwardSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_downwardSpeed -= m_frameTime * 0.005f;

		if (m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_positionY -= m_downwardSpeed;
}

void Position::TurnLeft(bool keydown)
{
	// Update the left turn speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if (m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime* 0.005f;

		if (m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationY -= m_leftTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if (m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}


}

void Position::TurnRight(bool keydown)
{
	// Update the right turn speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if (m_rightTurnSpeed > (m_frameTime * 0.15f))
		{
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime* 0.005f;

		if (m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationY += m_rightTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if (m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}

}

void Position::LookUpward(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_lookUpSpeed += m_frameTime * 7.5f;

		if (m_lookUpSpeed > (m_frameTime * 75.0f))
		{
			m_lookUpSpeed = m_frameTime * 75.0f;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime* 2.0f;

		if (m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationX -= m_lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if (m_rotationX > 90.0f)
	{
		m_rotationX = 90.0f;
	}

}

void Position::LookDownward(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_lookDownSpeed += m_frameTime * 7.5f;

		if (m_lookDownSpeed > (m_frameTime * 75.0f))
		{
			m_lookDownSpeed = m_frameTime * 75.0f;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime* 2.0f;

		if (m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationX += m_lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if (m_rotationX < -90.0f)
	{
		m_rotationX = -90.0f;
	}

}

void Position::MouseMove(int mouseX, int mouseY)
{
	m_rotationX += m_frameTime * mouseY * 0.2f;

	// Keep the rotation maximum 90 degrees.
	if (m_rotationX < -89.0f)
	{
		m_rotationX = -89.0f;
	}

	if (m_rotationX > 89.0f)
	{
		m_rotationX = 89.0f;
	}

	m_rotationY += m_frameTime * mouseX * 0.2f;
}
