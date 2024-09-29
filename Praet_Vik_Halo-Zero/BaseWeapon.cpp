#include "pch.h"
#include "BaseWeapon.h"

BaseWeapon::BaseWeapon(const Rectf& levelBoundaries, bool isHeldByPlayer, float scale) :
	m_LevelBoundaries{ levelBoundaries },
	m_IsHeldByPlayer{ isHeldByPlayer },
	m_Scale{ scale }
{
}

BaseWeapon::~BaseWeapon()
{
}
