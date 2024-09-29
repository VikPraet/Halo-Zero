#include "pch.h"
#include "BulletImpactBlue.h"

Texture* BulletImpactBlue::m_pTexture{ nullptr };
int BulletImpactBlue::instanceCounter{ };

BulletImpactBlue::BulletImpactBlue(const Point2f& position, float scale) :
	m_Position{ position },
	m_Scale{ scale },
	m_IsActive{ true },
	m_hasLoopedAnim{ },
	m_CurrentFrame{ },
	m_AccuSec{ },
	SECONDS_PER_FRAME{ 0.05f },
	NR_OF_FRAMES{ 5 }
{
	if (m_pTexture == nullptr) {
		m_pTexture = new Texture("Textures/bullet_impact_PlasmaBlue.png");
	}
	++instanceCounter;
}

BulletImpactBlue::~BulletImpactBlue()
{
	--instanceCounter;
	if (instanceCounter < 1) {
		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

void BulletImpactBlue::Draw() const
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
			m_Position.y - (frameWidth / 2) * m_Scale,
			frameWidth * m_Scale,
			frameWidth * m_Scale
		};

		m_pTexture->Draw(dstRect, srcRect);
	}
}

void BulletImpactBlue::Update(float elapsedSec)
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
