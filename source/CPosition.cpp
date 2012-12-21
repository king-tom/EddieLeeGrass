#include "CPosition.h"


CPosition::CPosition()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;
	
	m_rotationX = -3.0f;
	m_rotationY = -46.0f;
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


CPosition::CPosition(const CPosition& other)
{
}


CPosition::~CPosition()
{
}


void CPosition::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CPosition::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}


void CPosition::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
	return;
}


void CPosition::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
	return;
}


void CPosition::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}


void CPosition::MoveForward(bool keydown, float mouseX, float mouseY, bool shiftPressed)
{
	float radians;


	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		if(shiftPressed)
		{
			m_forwardSpeed += m_frameTime * 0.08f;

			if(m_forwardSpeed > (m_frameTime * .10f))
			{
				m_forwardSpeed = m_frameTime * .10f;
			}
		}
		else
		{
			m_forwardSpeed += m_frameTime * .0006f;

			if(m_forwardSpeed > (m_frameTime * .008f))
			{
				m_forwardSpeed = m_frameTime * .008f;
			}
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime * 0.00059f;

		if(m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = (mouseX - 46) * 0.0174532925f;

	// Update the position.
	m_positionX += sinf(radians) * m_forwardSpeed;
	m_positionZ += cosf(radians) * m_forwardSpeed;

	return;
}


void CPosition::MoveBackward(bool keydown, float mouseX, float mouseY, bool shiftPressed)
{
	float radians;

	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		if(shiftPressed)
		{
			m_backwardSpeed += m_frameTime * .08f;

			if(m_backwardSpeed > (m_frameTime * .1f))
			{
				m_backwardSpeed = m_frameTime * .1f;
			}
		}
		else
		{
			m_backwardSpeed += m_frameTime * .0006f;

			if(m_backwardSpeed > (m_frameTime * .008f))
			{
				m_backwardSpeed = m_frameTime * .008f;
			}
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * 0.00059f;
		
		if(m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = (mouseX - 46) * 0.0174532925f;

	// Update the position.
	m_positionX -= sinf(radians) * m_backwardSpeed;
	m_positionZ -= cosf(radians) * m_backwardSpeed;

	return;
}


void CPosition::MoveUpward(bool keydown)
{
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_upwardSpeed += m_frameTime * .08f;

		if(m_upwardSpeed > (m_frameTime * .10f))
		{
			m_upwardSpeed = m_frameTime * .10f;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * .02f;

		if(m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_positionY += m_upwardSpeed;

	return;
}


void CPosition::MoveDownward(bool keydown)
{
	// Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_downwardSpeed += m_frameTime * .08f;

		if(m_downwardSpeed > (m_frameTime * .10f))
		{
			m_downwardSpeed = m_frameTime * .10f;
		}
	}
	else
	{
		m_downwardSpeed -= m_frameTime * .02f;

		if(m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_positionY -= m_downwardSpeed;

	return;
}


void CPosition::TurnLeft(bool keydown, float mouseX, float mouseY, bool shiftPressed)
{
	// Update the left turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		if(shiftPressed)
		{
			m_leftTurnSpeed += m_frameTime * .08f;

			if(m_leftTurnSpeed > (m_frameTime * .1f))
			{
				m_leftTurnSpeed = m_frameTime * .1f;
			}
		}
		else
		{
			m_leftTurnSpeed += m_frameTime * .0006f;

			if(m_leftTurnSpeed > (m_frameTime * .008f))
			{
				m_leftTurnSpeed = m_frameTime * .008f;
			}
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime* 0.00059f;

		if(m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

		float radians;

		// Convert degrees to radians.
	radians = (mouseX - 90 - 46) * 0.0174532925f;

	// Update the position.
	m_positionX += sinf(radians) * m_leftTurnSpeed;
	m_positionZ += cosf(radians) * m_leftTurnSpeed;
	// Keep the rotation in the 0 to 360 range.
	/*if(m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}
	*/
	return;
}


void CPosition::TurnRight(bool keydown, float mouseX, float mouseY, bool shiftPressed)
{
	// Update the right turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		if(shiftPressed)
		{
			m_rightTurnSpeed += m_frameTime * 0.08f;

			if(m_rightTurnSpeed > (m_frameTime * 0.1f))
			{
				m_rightTurnSpeed = m_frameTime * 0.1f;
			}
		}
		else
		{
			m_rightTurnSpeed += m_frameTime * 0.0006f;

			if(m_rightTurnSpeed > (m_frameTime * 0.008f))
			{
				m_rightTurnSpeed = m_frameTime * 0.008f;
			}
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime* 0.00059f;

		if(m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	float radians;

		// Convert degrees to radians.
	radians = (mouseX + 90 - 46) * 0.0174532925f;

	// Update the position.
	m_positionX += sinf(radians) * m_rightTurnSpeed;
	m_positionZ += cosf(radians) * m_rightTurnSpeed;

	return;
}


void CPosition::LookUpward(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookUpSpeed += m_frameTime * 0.01f;

		if(m_lookUpSpeed > (m_frameTime * 0.15f))
		{
			m_lookUpSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime* 0.005f;

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


void CPosition::LookDownward(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookDownSpeed += m_frameTime * 0.01f;

		if(m_lookDownSpeed > (m_frameTime * 0.15f))
		{
			m_lookDownSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime* 0.005f;

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