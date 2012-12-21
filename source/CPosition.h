#pragma once

#include <math.h>


class CPosition
{
public:
	CPosition();
	CPosition(const CPosition&);
	~CPosition();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void GetPosition(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	void SetFrameTime(float);

	void MoveForward(bool, float, float, bool);
	void MoveBackward(bool, float, float, bool);
	void MoveUpward(bool);
	void MoveDownward(bool);
	void TurnLeft(bool, float, float, bool);
	void TurnRight(bool, float, float, bool);
	void LookUpward(bool);
	void LookDownward(bool);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	float m_frameTime;

	float m_forwardSpeed, m_backwardSpeed;
	float m_upwardSpeed, m_downwardSpeed;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;

};