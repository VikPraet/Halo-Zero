#include "pch.h"
#include "StrongPlasmaPistolBullet.h"
#include "Texture.h"
#include "Matrix2x3.h"
#include <iostream>

Texture* StrongPlasmaPistolBullet::m_pBulletTexture{ nullptr };
int StrongPlasmaPistolBullet::instanceCounter{ };

StrongPlasmaPistolBullet::StrongPlasmaPistolBullet(const Point2f& centerPos, float angle, bool isGoingLeft, float scale) :
	m_BulletDamage		{ 38.f },
	m_Shape				{ 0.f, 0.f, 16.f, 28.f },
	m_CenterPosition	{ },
	m_StartPos			{ centerPos },
	m_Angle				{ angle },
	m_Velocity			{ 270.f, 0.f },
	m_IsGoingLeft		{ isGoingLeft },
	m_Scale				{ scale },
	m_Vertices			{ Point2f{m_Shape.left, m_Shape.bottom},
						  Point2f{m_Shape.left + m_Shape.width, m_Shape.bottom},
						  Point2f{m_Shape.left + m_Shape.width, m_Shape.bottom + m_Shape.height},
						  Point2f{m_Shape.left, m_Shape.bottom + m_Shape.height} }
{
	if (m_pBulletTexture == nullptr) {
		m_pBulletTexture = new Texture("Textures/Bullet_Strong_PlasmaPistol.png");
	}
	++instanceCounter;
	m_Velocity.x *= m_Scale;

	// center spawnpos to gun
	m_StartPos.y -= 30;
}

StrongPlasmaPistolBullet::~StrongPlasmaPistolBullet()
{
	--instanceCounter;
	if (instanceCounter < 1)
	{
		delete m_pBulletTexture;
		m_pBulletTexture = nullptr;
	}
}

void StrongPlasmaPistolBullet::Draw() const
{
	if (m_pBulletTexture != nullptr)
	{
		Rectf bulletRect{
			-m_pBulletTexture->GetWidth() / 2,
			-m_pBulletTexture->GetHeight() / 2,
			m_pBulletTexture->GetWidth() * m_Scale,
			m_pBulletTexture->GetHeight() * m_Scale
		};

		glPushMatrix();
		{
			glTranslatef(m_StartPos.x, m_StartPos.y, 0.0f);
			if (m_IsGoingLeft)
			{
				glScalef(-1, 1, 1);
			}
			glRotatef(m_Angle, 0.0f, 0.0f, 1.0f);
			glTranslatef(m_CenterPosition.x, m_CenterPosition.y + m_Shape.height / 2, 0.0f);

			m_pBulletTexture->Draw(bulletRect);
		}
		glPopMatrix();

	}

	utils::SetColor(Color4f{ 0.f, 1.f, 0.3f, 1.f });
	//utils::DrawPolygon(m_TransformedVertices, true, 2.0f);
}

void StrongPlasmaPistolBullet::Update(float elapsedSec)
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

Point2f StrongPlasmaPistolBullet::GetCenterPos() const
{
	Point2f BulletCenterPos{
		m_TransformedVertices[0].x + m_Shape.width * m_Scale / 2,
		m_TransformedVertices[0].y + m_Shape.height * m_Scale / 2
	};
	return BulletCenterPos;
}

Point2f StrongPlasmaPistolBullet::GetLeftMiddlePos() const
{
	Point2f BulletLeftMiddlePos{
		m_TransformedVertices[0].x,
		m_TransformedVertices[0].y + m_Shape.height * m_Scale / 2
	};
	return BulletLeftMiddlePos;
}

Point2f StrongPlasmaPistolBullet::GetRightMiddlePos() const
{
	Point2f BulletRightMiddlePos{
	m_TransformedVertices[1].x,
	m_TransformedVertices[1].y + m_Shape.height * m_Scale / 2
	};
	return BulletRightMiddlePos;
}

float StrongPlasmaPistolBullet::GetBulletDamage() const
{
	return m_BulletDamage;
}

void StrongPlasmaPistolBullet::CleanUp()
{
	StrongPlasmaPistolBullet::~StrongPlasmaPistolBullet();
}