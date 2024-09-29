#include "pch.h"
#include "BaseGrenade.h"

BaseGrenade::BaseGrenade(const Point2f& starPos, bool isLookingLeft, float scale) :
	m_StartPos		{ starPos },
	m_IsLookingLeft	{ isLookingLeft },
	m_Scale			{ scale }
{
}

BaseGrenade::~BaseGrenade()
{
}
