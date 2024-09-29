#include "pch.h"
#include "AutoRifleBullet.h"
#include "Texture.h"
#include "Matrix2x3.h"
#include <iostream>

AutoRifleBullet::AutoRifleBullet(const Point2f& centerPos, float angle, bool isGoingLeft, float scale) :
	m_BulletDamage		{ 3.3f },
	m_Shape				{ 0.f, 0.f, 15.f, 6.f },
	m_CenterPosition	{ },
	m_StartPos			{ centerPos },
	m_Angle				{ angle },
	m_Velocity			{ 650, 0.f },
	m_IsGoingLeft		{isGoingLeft},
	m_Scale				{ scale },
	m_Vertices			{ Point2f{m_Shape.left, m_Shape.bottom},
						  Point2f{m_Shape.left + m_Shape.width, m_Shape.bottom},
						  Point2f{m_Shape.left + m_Shape.width, m_Shape.bottom + m_Shape.height},
						  Point2f{m_Shape.left, m_Shape.bottom + m_Shape.height} }
{
	m_Velocity.x *= m_Scale;
}

AutoRifleBullet::~AutoRifleBullet()
{

}

void AutoRifleBullet::Draw() const
{
	//utils::SetColor(Color4f{ 1.f, 0.f, 1.f, 1.f });
	//utils::DrawPolygon(m_TransformedVertices, true, 2.0f);
}

void AutoRifleBullet::Update(float elapsedSec)
{
	m_CenterPosition += m_Velocity * m_Scale * elapsedSec;

	Matrix2x3 matTranslateStartPos, matTranslate, matRotate, matScaleBack, matScale;
	matTranslate.SetAsTranslate(m_CenterPosition.x, m_CenterPosition.y);
	matTranslateStartPos.SetAsTranslate(m_StartPos.x, m_StartPos.y);
	matRotate.SetAsRotate(m_Angle);
	matScale.SetAsScale(m_Scale);

	if (m_IsGoingLeft) {
		matScaleBack.SetAsScale(-1.f, 1.f);
	}
	else {
		matScaleBack.SetAsScale(1.f, 1.f);
	}

	Matrix2x3 matTransform = matTranslateStartPos * matScaleBack * matRotate * matTranslate * matScale;
	m_TransformedVertices = matTransform.Transform(m_Vertices);
}

Point2f AutoRifleBullet::GetCenterPos() const
{
	Point2f BulletCenterPos{
		m_TransformedVertices[0].x + m_Shape.width * m_Scale / 2,
		m_TransformedVertices[0].y + m_Shape.height * m_Scale / 2
	};
	return BulletCenterPos;
}

Point2f AutoRifleBullet::GetLeftMiddlePos() const
{
	Point2f BulletLeftMiddlePos{
		m_TransformedVertices[0].x,
		m_TransformedVertices[0].y + m_Shape.height * m_Scale / 2
	};
	return BulletLeftMiddlePos;
}

Point2f AutoRifleBullet::GetRightMiddlePos() const
{
	Point2f BulletRightMiddlePos{
	m_TransformedVertices[1].x,
	m_TransformedVertices[1].y + m_Shape.height * m_Scale / 2
	};
	return BulletRightMiddlePos;
}

float AutoRifleBullet::GetBulletDamage() const
{
	return m_BulletDamage;
}

void AutoRifleBullet::CleanUp()
{
	AutoRifleBullet::~AutoRifleBullet();
}