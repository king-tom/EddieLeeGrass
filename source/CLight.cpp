#include "CLight.h"


CLight::CLight()
{
}


CLight::CLight(const CLight& other)
{
}


CLight::~CLight()
{
}


void CLight::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void CLight::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void CLight::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}


D3DXVECTOR4 CLight::GetAmbientColor()
{
	return m_ambientColor;
}


D3DXVECTOR4 CLight::GetDiffuseColor()
{
	return m_diffuseColor;
}


D3DXVECTOR3 CLight::GetDirection()
{
	return m_direction;
}