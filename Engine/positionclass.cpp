////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "positionclass.h"

PositionClass::PositionClass()
{
	DEGREE_TO_RADIAN_CONVERSION_RATE = 0.0174532925f;
	
	MOVE_ACCELERATION = 0.0015f;
	MOVE_DECELERATION = 0.003f;
	MOVE_MAXSPEED = 0.0125f;
	//MOVE_MAXSPEED = 3;

	LOOK_ACCELERATION  = 0.02f;
	LOOK_DECELERATION  = 0.01f;
	LOOK_MAXSPEED = 0.1f;

	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;
	
	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_frameTime = 0.0f;

	m_forwardSpeed   = 0.0f;
	m_backwardSpeed  = 0.0f;
	m_upwardSpeed    = 0.0f;
	m_downwardSpeed  = 0.0f;
	m_leftTurnSpeed  = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed    = 0.0f;
	m_lookDownSpeed  = 0.0f;
}

PositionClass::PositionClass(const PositionClass& other)
{
}

PositionClass::~PositionClass()
{
}

void PositionClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

void PositionClass::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
	return;
}

void PositionClass::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
	return;
}

void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}

void PositionClass::MoveForward(bool keydown)
{
	float radians;
	float radiansX;

	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_forwardSpeed += m_frameTime * MOVE_ACCELERATION;//accel rate

		if(m_forwardSpeed > (m_frameTime * MOVE_MAXSPEED))
		{
			m_forwardSpeed = m_frameTime * MOVE_MAXSPEED;
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime * MOVE_DECELERATION;

		if(m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotationY * DEGREE_TO_RADIAN_CONVERSION_RATE;
	radiansX = m_rotationX * DEGREE_TO_RADIAN_CONVERSION_RATE;

	// Update the position.
	m_positionX += sinf(radians) * m_forwardSpeed * cosf(radiansX);
	m_positionZ += cosf(radians) * m_forwardSpeed * cosf(radiansX);
	m_positionY -= sinf(radiansX) * m_forwardSpeed;

	return;
}

void PositionClass::StrafeLeft(bool keydown)
{
	float radians;

	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_leftSpeed += m_frameTime * MOVE_ACCELERATION;//accel rate

		if (m_leftSpeed > (m_frameTime * MOVE_MAXSPEED))
		{
			m_leftSpeed = m_frameTime * MOVE_MAXSPEED;
		}
	}
	else
	{
		m_leftSpeed -= m_frameTime * MOVE_DECELERATION;

		if (m_leftSpeed < 0.0f)
		{
			m_leftSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotationY * DEGREE_TO_RADIAN_CONVERSION_RATE;

	// Update the position.
	m_positionZ += sinf(radians) * m_leftSpeed;
	m_positionX -= cosf(radians) * m_leftSpeed;

	return;

}

void PositionClass::StrafeRight(bool keydown)
{
	float radians;

	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_rightSpeed += m_frameTime * MOVE_ACCELERATION;//accel rate

		if (m_rightSpeed > (m_frameTime * MOVE_MAXSPEED))
		{
			m_rightSpeed = m_frameTime * MOVE_MAXSPEED;
		}
	}
	else
	{
		m_rightSpeed -= m_frameTime * MOVE_DECELERATION;

		if (m_rightSpeed < 0.0f)
		{
			m_rightSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotationY * DEGREE_TO_RADIAN_CONVERSION_RATE;

	// Update the position.
	m_positionZ -= sinf(radians) * m_rightSpeed;
	m_positionX += cosf(radians) * m_rightSpeed;

	return;
}

void PositionClass::MoveBackward(bool keydown)
{
	float radians;
	float radiansX;

	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_backwardSpeed += m_frameTime * MOVE_ACCELERATION;//accel rate

		if(m_backwardSpeed > (m_frameTime * MOVE_MAXSPEED))
		{
			m_backwardSpeed = m_frameTime * MOVE_MAXSPEED;
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * MOVE_DECELERATION;
		
		if(m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotationY * DEGREE_TO_RADIAN_CONVERSION_RATE;
	radiansX = m_rotationX * DEGREE_TO_RADIAN_CONVERSION_RATE;
	// Update the position.

	//m_positionX -= sinf(radians) * m_backwardSpeed * sinf(radiansX);
	//m_positionZ -= cosf(radians) * m_backwardSpeed * sinf(radians);

	//m_positionY += sinf(radiansX) * m_backwardSpeed;


	m_positionX -= sinf(radians) * m_backwardSpeed * cosf(radiansX);
	m_positionZ -= cosf(radians) * m_backwardSpeed * cosf(radiansX);
	m_positionY += sinf(radiansX) * m_backwardSpeed;

	return;
}

void PositionClass::MoveUpward(bool keydown)
{
	float radians;
	float radiansX;
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_upwardSpeed += m_frameTime * MOVE_ACCELERATION;//accel rate 2

		if(m_upwardSpeed > (m_frameTime * MOVE_MAXSPEED))//max speed
		{
			m_upwardSpeed = m_frameTime * MOVE_MAXSPEED;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * MOVE_DECELERATION;//decel rate 2

		if(m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}

	radians = m_rotationY * DEGREE_TO_RADIAN_CONVERSION_RATE;
	radiansX = m_rotationX * DEGREE_TO_RADIAN_CONVERSION_RATE;

	// Update the height position.
	m_positionX -= sinf(radians) * m_upwardSpeed * sinf(radiansX);
	m_positionZ -= cosf(radians) * m_upwardSpeed * sinf(radiansX);

	m_positionY += m_upwardSpeed * cosf(radiansX);

	return;
}

void PositionClass::MoveDownward(bool keydown)
{
	float radians;
	float radiansX;
	// Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_downwardSpeed += m_frameTime * MOVE_ACCELERATION;//accel rate 2

		if(m_downwardSpeed > (m_frameTime * MOVE_MAXSPEED))
		{
			m_downwardSpeed = m_frameTime * MOVE_MAXSPEED;
		}
	}
	else
	{
		m_downwardSpeed -= m_frameTime * MOVE_DECELERATION;

		if(m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}

	radians = m_rotationY * DEGREE_TO_RADIAN_CONVERSION_RATE;
	radiansX = m_rotationX * DEGREE_TO_RADIAN_CONVERSION_RATE;

	// Update the height position.
	m_positionX += sinf(radians) * m_upwardSpeed * sinf(radiansX);
	m_positionZ += cosf(radians) * m_upwardSpeed * sinf(radiansX);

	m_positionY -= m_downwardSpeed * cosf(radiansX);

	return;
}

void PositionClass::TurnLeft(bool keydown)
{
	// Update the left turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_leftTurnSpeed += m_frameTime * LOOK_ACCELERATION;//accel rate

		if(m_leftTurnSpeed > (m_frameTime * LOOK_MAXSPEED))//max speed
		{
			m_leftTurnSpeed = m_frameTime * LOOK_MAXSPEED;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime* LOOK_DECELERATION;//decel rate

		if(m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationY -= m_leftTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}

	return;
}

void PositionClass::TurnRight(bool keydown)
{
	// Update the right turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_rightTurnSpeed += m_frameTime * LOOK_ACCELERATION;//accel rate

		if(m_rightTurnSpeed > (m_frameTime * LOOK_MAXSPEED))//max speed
		{
			m_rightTurnSpeed = m_frameTime * LOOK_MAXSPEED;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime* LOOK_DECELERATION;//decel rate

		if(m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationY += m_rightTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}

	return;
}

void PositionClass::LookUpward(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookUpSpeed += m_frameTime * LOOK_ACCELERATION;//accel rate

		if(m_lookUpSpeed > (m_frameTime * LOOK_MAXSPEED))
		{
			m_lookUpSpeed = m_frameTime * LOOK_MAXSPEED;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime* LOOK_DECELERATION;

		if(m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationX -= m_lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_rotationX > 90.0f)
	{
		m_rotationX = 90.0f;
	}

	return;
}

void PositionClass::LookDownward(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookDownSpeed += m_frameTime * LOOK_ACCELERATION;//accel rate

		if(m_lookDownSpeed > (m_frameTime * LOOK_MAXSPEED))
		{
			m_lookDownSpeed = m_frameTime * LOOK_MAXSPEED;//max speed
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime* LOOK_DECELERATION;

		if(m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationX += m_lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_rotationX < -90.0f)
	{
		m_rotationX = -90.0f;
	}

	return;
}
void PositionClass::MouseRotate(int mouseX, int mouseY)
{
	float MOUSE_SENSITIVITY = 0.1f;

	if ((m_rotationX + (mouseY * MOUSE_SENSITIVITY)) > 90) m_rotationX = 90;
	else if ((m_rotationX + (mouseY * MOUSE_SENSITIVITY)) < -90) m_rotationX = -90;
	else m_rotationX += (mouseY * MOUSE_SENSITIVITY);

	m_rotationY += (mouseX * MOUSE_SENSITIVITY);

	//if ((m_rotationX + mouseY * MOUSE_SENSITIVITY) > 90)
	//	m_rotationX = 90;
	//else if ((m_rotationX + mouseY * MOUSE_SENSITIVITY) < -90)
	//	m_rotationX = -90;
	//else
	//	m_rotationX += mouseY * MOUSE_SENSITIVITY;

	//m_rotationY += mouseX * MOUSE_SENSITIVITY;
	//if (m_rotationY > 360)
	//	m_rotationY -= 360;
	//if (m_rotationY  < 0)
	//	m_rotationY += 360;

	//m_rotationY += mouseX* MOUSE_SENSITIVITY;

	//m_rotationX += mouseY * MOUSE_SENSITIVITY;
	//m_rotationY += mouseX * MOUSE_SENSITIVITY;

	//if (m_rotationY > 360.0f)
	//{
	//	m_rotationY -= 360.0f;
	//}
	//if (m_rotationY < 0.0f)
	//{
	//	m_rotationY += 360.0f;
	//}
	//if (m_rotationX < -90.0f)
	//{
	//	m_rotationX = -90.0f;
	//}
	//if (m_rotationX > 90.0f)
	//{
	//	m_rotationX = 90.0f;
	//}
}