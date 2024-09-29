#pragma once
#include "utils.h"
class Wall
{
public:
	Wall(const Rectf& dimensions, float scale);
	~Wall();

	void HandleCollision(Rectf& actorShape, Vector2f& actorVelocity);
	std::vector<Point2f> GetVertices() const;

private:
	const float m_Scale;
	Rectf m_Dimensions;

	std::vector<Point2f> m_Vertices;
	utils::HitInfo hitInfo;
};