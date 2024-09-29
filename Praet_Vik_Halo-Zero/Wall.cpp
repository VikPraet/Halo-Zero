#include "pch.h"
#include "Wall.h"

Wall::Wall(const Rectf& dimensions, float scale):
	m_Scale		 { scale },
	m_Dimensions { dimensions },
	m_Vertices	 { },
	hitInfo		 { }
{
	m_Dimensions.left *= m_Scale;
	m_Dimensions.bottom *= m_Scale;
	m_Dimensions.width *= m_Scale;
	m_Dimensions.height *= m_Scale;

	m_Vertices.push_back(Point2f{ m_Dimensions.left , m_Dimensions.bottom });
	m_Vertices.push_back(Point2f{ m_Dimensions.left + m_Dimensions.width , m_Dimensions.bottom });
	m_Vertices.push_back(Point2f{ m_Dimensions.left + m_Dimensions.width , m_Dimensions.bottom + m_Dimensions.height });
	m_Vertices.push_back(Point2f{ m_Dimensions.left , m_Dimensions.bottom + m_Dimensions.height });
}

Wall::~Wall()
{
}

void Wall::HandleCollision(Rectf& actorShape, Vector2f& actorVelocity)
{
	float margin{ 5.f };
	// collide top lefside
	if (utils::Raycast(m_Vertices,
		Point2f{ actorShape.left + margin, actorShape.bottom + actorShape.height },
		Point2f{ actorShape.left + margin, actorShape.bottom },
		hitInfo) && actorVelocity.y > 0)
	{
		actorShape.bottom = hitInfo.intersectPoint.y - actorShape.height;
		actorVelocity.y = 0;
	}
	// collide top rightside
	if (utils::Raycast(m_Vertices,
		Point2f{ actorShape.left + actorShape.width - margin, actorShape.bottom + actorShape.height },
		Point2f{ actorShape.left + actorShape.width - margin, actorShape.bottom },
		hitInfo) && actorVelocity.y > 0)
	{
		actorShape.bottom = hitInfo.intersectPoint.y - actorShape.height;
		actorVelocity.y = 0;
	}

	// collide left topside
	if (utils::Raycast(m_Vertices,
		Point2f{ actorShape.left, actorShape.bottom + margin },
		Point2f{ actorShape.left + (actorShape.width / 2), actorShape.bottom + margin },
		hitInfo))
	{
		actorShape.left = hitInfo.intersectPoint.x;
		actorVelocity.x = 0;
	}
	// collide left bottomside
	if (utils::Raycast(m_Vertices,
		Point2f{ actorShape.left, actorShape.bottom + actorShape.height - margin },
		Point2f{ actorShape.left + (actorShape.width / 2), actorShape.bottom + actorShape.height - margin },
		hitInfo))
	{
		actorShape.left = hitInfo.intersectPoint.x;
		actorVelocity.x = 0;
	}

	// collide right topside
	if (utils::Raycast(m_Vertices,
		Point2f{ actorShape.left + (actorShape.width / 2), actorShape.bottom + margin },
		Point2f{ actorShape.left + actorShape.width, actorShape.bottom + margin },
		hitInfo))
	{
		actorShape.left = hitInfo.intersectPoint.x - actorShape.width - 1;
		actorVelocity.x = 0;
	}
	// collide right bottomside
	if (utils::Raycast(m_Vertices,
		Point2f{ actorShape.left + (actorShape.width / 2), actorShape.bottom + actorShape.height - margin },
		Point2f{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height - margin },
		hitInfo))
	{
		actorShape.left = hitInfo.intersectPoint.x - actorShape.width - 1;
		actorVelocity.x = 0;
	}
}

std::vector<Point2f> Wall::GetVertices() const
{
	return m_Vertices;
}
