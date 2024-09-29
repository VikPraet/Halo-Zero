#pragma once
#include "utils.h"
#include <vector>
#include "Texture.h"

class Wall;
class Level
{
public:
	Level(float scale = 1);
	~Level();
	void DrawParallaxTwo() const;
	void DrawParallaxOne() const;
	void DrawBackground() const;
	void DrawForeground() const;
	void HandleCollision(Rectf& actorShape, Vector2f& actorVelocity);
	bool IsOnGround(const Rectf& actorShape);

	Point2f GetItersectionPoint() const;
	Rectf GetLevelRectf() const;
	std::vector<std::vector<Point2f>> GetVertices() const;
	std::vector<Wall*> GetWalls() const;
	std::vector<Point2f> GetWallVert(int idx) const;

	bool HasReachedEnd(const Rectf& actorShape);

private:
	Texture* m_pParallaxTwo;
	Texture* m_pParallaxOne;
	Texture* m_pBackGround;
	Texture* m_pForeGround;
	std::vector<std::vector<Point2f>> m_Vertices;

	utils::HitInfo	hitInfo;
	const float		m_Scale;
	Rectf			m_DestenationRect;
	const float		m_EndPosition;

	float			m_DistanceFromMiddle;

	// walls
	std::vector<Wall*> m_pWalls;
};