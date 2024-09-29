#include "pch.h"
#include "BulletImpactYellow.h"

Texture* BulletImpactYellow::m_pTexture{ nullptr };
int BulletImpactYellow::instanceCounter{ };

BulletImpactYellow::BulletImpactYellow(const Point2f& position, float scale) :
	m_Position			{ position },
	m_Scale				{ scale },
	m_IsActive			{ true },
	m_hasLoopedAnim		{ },
	m_CurrentFrame		{ },
	m_AccuSec			{ },
	SECONDS_PER_FRAME	{ 0.05f },
	NR_OF_FRAMES		{ 5 }
{
	if (m_pTexture == nullptr) {
		m_pTexture = new Texture("Textures/bullet_impact_Normal.png");
	}
	++instanceCounter;
}

BulletImpactYellow::~BulletImpactYellow()
{
	--instanceCounter;
	if (instanceCounter < 1) {
		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

void BulletImpactYellow::Draw() const
{
	if (m_IsActive)
	{
		Rectf srcRect{};
		const float frameWidth{ m_pTexture->GetWidth() / NR_OF_FRAMES };
		const float fraweHeigth{ m_pTexture->GetHeight() };

		srcRect.left = frameWidth * m_CurrentFrame;
		srcRect.bottom = fraweHeigth;
		srcRect.width = frameWidth;
		srcRect.height = fraweHeigth;

		Rectf dstRect{
			m_Position.x - (frameWidth / 2) * m_Scale,
			m_Position.y - (frameWidth / 2)* m_Scale,
			frameWidth * m_Scale,
			frameWidth * m_Scale
		};

		m_pTexture->Draw(dstRect, srcRect);
	}
}

void BulletImpactYellow::Update(float elapsedSec)
{
	m_AccuSec += elapsedSec;
	int nrFrames{ int(m_AccuSec / SECONDS_PER_FRAME) };
	m_CurrentFrame = nrFrames % NR_OF_FRAMES;

	if (m_hasLoopedAnim and m_CurrentFrame == 0)
	{
		m_IsActive = false;
	}

	if (m_CurrentFrame == NR_OF_FRAMES - 1) {
		m_hasLoopedAnim = true;
	}
}