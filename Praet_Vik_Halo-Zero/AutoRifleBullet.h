#pragma once
#include "BaseBullet.h"
#include "utils.h"
#include "Texture.h"
#include <cmath>

class AutoRifleBullet: public BaseBullet
{
public:
	AutoRifleBullet(const Point2f& centerPos, float angle, bool isGoingLeft, float scale);
	~AutoRifleBullet();

	void Draw() const;
	void Update(float elapsedSec);

	Point2f GetCenterPos() const;
	Point2f GetLeftMiddlePos() const;
	Point2f GetRightMiddlePos() const;
	float GetBulletDamage() const;

private:
	float		m_BulletDamage;
	Rectf		m_Shape;
	Point2f		m_CenterPosition;
	Point2f		m_StartPos;
	float		m_Angle;
	Vector2f	m_Velocity;
	bool		m_IsGoingLeft;
	float		m_Scale;

	std::vector<Point2f> m_Vertices;
	std::vector<Point2f> m_TransformedVertices;

	void CleanUp();
};