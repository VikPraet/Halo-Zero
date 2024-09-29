#pragma once
#include "utils.h"

class Camera
{
public:
	Camera(float width, float height);
	void SetLevelBoundaries(const Rectf& levelBoundaries);
	Point2f Translate(const Rectf& target) const;
	void TranslateParallaxOne(const Rectf& target) const;
	void TranslateParallaxTwo(const Rectf& target) const;

	void Update(float elapsedSec, const Rectf& target);

private:
	float m_Width;
	float m_Heigth;
	Rectf m_LevelBoundaries;

	Point2f m_CameraPos;
	float m_DistanceFromBordes;

	Point2f Track(const Rectf& target) const;
	void Clamp(Point2f& bottomLeft) const;
};