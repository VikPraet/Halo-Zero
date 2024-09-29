#include "pch.h"
#include "bullet_impact_BigBlue.h"

Texture* bullet_impact_BigBlue::m_pTexture{ nullptr };
int bullet_impact_BigBlue::instanceCounter{ };

bullet_impact_BigBlue::bullet_impact_BigBlue(const Point2f& position, float scale) :
	m_Position{ position },
	m_Scale{ scale },
	m_IsActive{ true },
	m_hasLoopedAnim{ },
	m_CurrentFrame{ },
	m_AccuSec{ },
	SECONDS_PER_FRAME{ 0.07f },
	NR_OF_FRAMES{ 5 }
{
	if (m_pTexture == nullptr) {
		m_pTexture = new Texture("Textures/bullet_impact_BigBlue.png");
	}
	++instanceCounter;
	m_Scale *= 1.25f;
}

bullet_impact_BigBlue::~bullet_impact_BigBlue()
{
	--instanceCounter;
	if (instanceCounter < 1) {
		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

void bullet_impact_BigBlue::Draw() const
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

void bullet_impact_BigBlue::Update(float elapsedSec)
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
