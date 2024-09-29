#include "pch.h"
#include "BaseObject.h"

BaseObject::BaseObject(const Point2f& position, float scale):
	m_Position	{ position },
	m_Scale		{ scale }
{
}

BaseObject::~BaseObject()
{
}
