#include "pch.h"
#include "Level.h"
#include "SVGParser.h"
#include "Wall.h"

Level::Level(float scale):
	m_pParallaxTwo		{ new Texture("Textures/Parallax_2.png")},
	m_pParallaxOne		{ new Texture("Textures/Parallax_1.png")},
	m_pBackGround		{ new Texture("Textures/BACKGROUND.png") },
	m_pForeGround		{ new Texture("Textures/FOREGROUND.png") },
	m_Vertices			{ },
	hitInfo				{ },
	m_Scale				{ scale },
	m_DestenationRect	{ },
	m_EndPosition		{ 11745.0f * m_Scale },
	m_DistanceFromMiddle{ },
	m_pWalls			{ }
{
	SVGParser::GetVerticesFromSvgFile("Textures/LEVEL_COLLISION.svg", m_Vertices);
	for (int i{}; i < m_Vertices[0].size(); ++i)
	{
		m_Vertices[0][i].x *= m_Scale;
		m_Vertices[0][i].y *= m_Scale;
	}

	m_DestenationRect.width  = m_pBackGround->GetWidth()  * m_Scale;
	m_DestenationRect.height = m_pBackGround->GetHeight() * m_Scale;

	m_pWalls.push_back(new Wall(Rectf{ 4442, 540, 76, 128 }, m_Scale));
	m_pWalls.push_back(new Wall(Rectf{ 5412, 540, 76, 128 }, m_Scale));
	m_pWalls.push_back(new Wall(Rectf{ 9862, 204, 76, 464 }, m_Scale));
	m_pWalls.push_back(new Wall(Rectf{ 10380, 204, 76, 464 }, m_Scale));

	m_pWalls.push_back(new Wall(Rectf{ -5, 0, 5, 668 }, m_Scale));
	m_pWalls.push_back(new Wall(Rectf{ 12400, 0, 5, 668 }, m_Scale));
}

Level::~Level()
{
	delete m_pParallaxTwo;
	m_pParallaxTwo = nullptr;

	delete m_pParallaxOne;
	m_pParallaxOne = nullptr;

	delete m_pBackGround;
	m_pBackGround = nullptr;

	delete m_pForeGround;
	m_pForeGround = nullptr;

	for (int i{}; i < m_pWalls.size(); ++i)
	{
		delete m_pWalls[i];
		m_pWalls[i] = nullptr;
	}
}

void Level::DrawParallaxTwo() const
{
	Rectf dstRect{
		0,
		50 * m_Scale,
		m_pParallaxTwo->GetWidth() * m_Scale,
		m_pParallaxTwo->GetHeight() * m_Scale
	};
	m_pParallaxTwo->Draw(dstRect);
}

void Level::DrawParallaxOne() const
{
	Rectf dstRect{
		0,
		0,
		m_pParallaxOne->GetWidth() * m_Scale,
		m_pParallaxOne->GetHeight() * m_Scale
	};
	m_pParallaxOne->Draw(dstRect);
}

void Level::DrawBackground() const
{
	m_pBackGround->Draw(m_DestenationRect);
}

void Level::DrawForeground() const
{
	m_pForeGround->Draw(m_DestenationRect);
	//utils::SetColor(Color4f{ 0.0f, 1.0f, 1.0f, 1.0f });
	//utils::DrawPolygon(m_Vertices[0]);
}

void Level::HandleCollision(Rectf& actorShape, Vector2f& actorVelocity)
{
	m_DistanceFromMiddle = actorShape.width * 0.23f;

	// collide Bottom
	if (utils::Raycast(m_Vertices[0],
		Point2f{ actorShape.left + (actorShape.width / 2) - m_DistanceFromMiddle, actorShape.bottom + actorShape.height },
		Point2f{ actorShape.left + (actorShape.width / 2) - m_DistanceFromMiddle, actorShape.bottom },
		hitInfo) && actorVelocity.y < 0)
	{
		actorShape.bottom = hitInfo.intersectPoint.y - 1;
		actorVelocity.y = 0;
	}
	else if (utils::Raycast(m_Vertices[0],
		Point2f{ actorShape.left + (actorShape.width / 2) + m_DistanceFromMiddle, actorShape.bottom + actorShape.height },
		Point2f{ actorShape.left + (actorShape.width / 2) + m_DistanceFromMiddle, actorShape.bottom },
		hitInfo) && actorVelocity.y < 0)
	{
		actorShape.bottom = hitInfo.intersectPoint.y - 1;
		actorVelocity.y = 0;
	}

	// collide left
	if (utils::Raycast(m_Vertices[0],
		Point2f{ actorShape.left, actorShape.bottom + (actorShape.height / 4) },
		Point2f{ actorShape.left + (actorShape.width / 2), actorShape.bottom + (actorShape.height / 4) },
		hitInfo))
	{
		actorShape.left = hitInfo.intersectPoint.x + 1;
		actorVelocity.x = 0;
	}

	// collide right
	if (utils::Raycast(m_Vertices[0],
		Point2f{ actorShape.left + (actorShape.width / 2), actorShape.bottom + (actorShape.height / 4) },
		Point2f{ actorShape.left + actorShape.width, actorShape.bottom + (actorShape.height / 4) },
		hitInfo))
	{
		actorShape.left = hitInfo.intersectPoint.x - actorShape.width - 1;
		actorVelocity.x = 0;
	}

	for (int i{}; i < m_pWalls.size(); ++i)
	{
		m_pWalls[i]->HandleCollision(actorShape, actorVelocity);
	}
}

bool Level::IsOnGround(const Rectf& actorShape)
{
	return (utils::Raycast(m_Vertices[0],
			Point2f{ actorShape.left + (actorShape.width / 2) - m_DistanceFromMiddle, actorShape.bottom + actorShape.height },
			Point2f{ actorShape.left + (actorShape.width / 2) - m_DistanceFromMiddle, actorShape.bottom },
			hitInfo) or 
			utils::Raycast(m_Vertices[0],
			Point2f{ actorShape.left + (actorShape.width / 2) + m_DistanceFromMiddle, actorShape.bottom + actorShape.height },
			Point2f{ actorShape.left + (actorShape.width / 2) + m_DistanceFromMiddle, actorShape.bottom },
			hitInfo));
}

Point2f Level::GetItersectionPoint() const
{
	return hitInfo.intersectPoint;
}

Rectf Level::GetLevelRectf() const
{
	return m_DestenationRect;
}

std::vector<std::vector<Point2f>> Level::GetVertices() const
{
	return m_Vertices;
}

std::vector<Wall*> Level::GetWalls() const
{
	return m_pWalls;
}

std::vector<Point2f> Level::GetWallVert(int idx) const
{
	return m_pWalls[idx]->GetVertices();
}

bool Level::HasReachedEnd(const Rectf& actorShape)
{
	return (actorShape.left > m_EndPosition);
}