#pragma once
#include "utils.h"
class Level;
class Texture;
class BulletCasing
{
public:
	BulletCasing(Point2f position, bool isActorFacingLeft, float shootingAngle, float scale);
	~BulletCasing();

	void Update(float elapsedSec, Level& level);

	void Draw() const;

private:
	float		m_Scale;
	Texture*	m_pTexture;
	Rectf		m_Shape;
	Vector2f	m_Acceleration;
	Vector2f	m_Velocity;
	float		m_Angle;
	float		m_rotationSpeed;
	float		m_BounceVelocity;
	float		m_BouncePercent;
	Point2f		m_StartPos;
};

