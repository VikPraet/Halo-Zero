#include "pch.h"
#include "BulletCasing.h"
#include "Level.h"
#include "Texture.h"

BulletCasing::BulletCasing(Point2f position, bool isActorFacingLeft, float shootingAngle, float scale) :
	m_Scale			{ scale },
	m_pTexture		{ new Texture("Textures/BulletCasing.png") },
	m_Shape			{ },
	m_Acceleration	{ 200.f , -1350.f },
	m_Velocity		{ -370.f, 170.f },
	m_Angle			{ },
	m_rotationSpeed	{ float(rand()% 500 + 1000) },
	m_BounceVelocity{ 200.f },
	m_BouncePercent	{ 0.8f },
	m_StartPos		{ position }
{
	m_Shape.left = position.x;
	m_Shape.bottom = position.y;
	m_Shape.width = m_pTexture->GetWidth() * scale;
	m_Shape.height = m_pTexture->GetHeight() * scale;

	if (isActorFacingLeft)
	{
		m_Acceleration.x *= -1;
		m_Velocity.x	 *= -1;
		m_rotationSpeed *= -1;
	}

	int MaxRandomExtraVelocoty{ 40 };
	m_Velocity.x += -rand()% MaxRandomExtraVelocoty;
	m_Velocity.y += rand()% MaxRandomExtraVelocoty;
}

BulletCasing::~BulletCasing()
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void BulletCasing::Update(float elapsedSec, Level& level)
{
	level.HandleCollision(m_Shape, m_Velocity);

	m_Velocity.y += m_Acceleration.y * elapsedSec;
	m_Shape.bottom += m_Velocity.y * elapsedSec;

	if (abs(m_Velocity.x) > 5)
	{
		m_Velocity.x += m_Acceleration.x * elapsedSec;
		m_Shape.left += m_Velocity.x * elapsedSec;

		if (level.IsOnGround(m_Shape))
		{
			m_Velocity.y += m_BounceVelocity *= m_BouncePercent;
			m_rotationSpeed *= m_BouncePercent;
		}

		m_Angle += m_rotationSpeed * elapsedSec;

	}
}

void BulletCasing::Draw() const
{
	Rectf dstRect{
		0.f,
		0.f,
		m_Shape.width,
		m_Shape.height
	};


	glPushMatrix();

	glTranslatef(m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2, 0.0f);
	glRotatef(m_Angle,0,0,1);
	glTranslatef(-m_Shape.width / 2, -m_Shape.height / 2, 0.0f);

	m_pTexture->Draw(dstRect);

	glPopMatrix();
}
