#pragma once
#include "BaseBullet.h"
#include "utils.h"
#include "Texture.h"
#include <cmath>

class PlasmaRifleBullet: public BaseBullet
{
public:
	PlasmaRifleBullet(const Point2f& centerPos, float angle, bool isGoingLeft, float scale);
	~PlasmaRifleBullet();

	void Draw() const override;
	void Update(float elapsedSec) override;

	Point2f GetCenterPos() const override;
	Point2f GetLeftMiddlePos() const override;
	Point2f GetRightMiddlePos() const override;
	float GetBulletDamage() const;

	void CleanUp() override;

private:
	static Texture* m_pBulletTexture;
	static int instanceCounter;

	float m_BulletDamage;
	Rectf m_Shape;
	Point2f m_CenterPosition;
	Point2f m_StartPos;
	float m_Angle;
	Vector2f m_Velocity;
	bool m_IsGoingLeft;
	float m_Scale;

	std::vector<Point2f> m_Vertices;
	std::vector<Point2f> m_TransformedVertices;
};

