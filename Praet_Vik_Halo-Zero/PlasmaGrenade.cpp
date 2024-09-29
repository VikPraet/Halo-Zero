#include "pch.h"
#include "PlasmaGrenade.h"
#include "Texture.h"
#include "BaseEnemy.h"
#include "Avatar.h"

// texture data
Texture* PlasmaGrenade::m_pGrenadeTexture{ nullptr };
Texture* PlasmaGrenade::m_pExplosionTexture{ nullptr };
SoundEffect* PlasmaGrenade::m_pExpolsionEffect{ nullptr };
int PlasmaGrenade::instanceCounter{ };

PlasmaGrenade::PlasmaGrenade(const Point2f& starPos, bool isLookingLeft, float scale) :
	BaseGrenade(starPos, isLookingLeft, scale),
	// animation data
	m_IsActive				{ true },
	m_hasLoopedAnim			{ false },
	m_CurrentFrame			{ },
	m_AccuSec				{ },
	NR_OF_EXPLOSION_FRAMES	{ 6 },
	SECONDS_PER_FRAME		{ 0.1f },

	// grenade data
	m_Shape					{ },
	m_IsOnGround			{ false },
	m_HasHitGround			{ false },
	BOUNCE_FACTOR			{ 0.4f },
	TIME_TO_EXPLODE			{ 2.f }, //seconds
	m_ExplosionTime			{ },
	m_IsExplodiong			{ false },
	m_IsAtached				{ false },
	m_AtachedIdx			{ },

	// damage data
	m_CheckHitCounterEnemies{ },
	m_CheckHitCounterAvatar	{ },
	m_GrenadeDamage			{ 75.f },
	m_Radius				{ 40.f * m_Scale },

	// speed
	m_BounceVelocity		{ 185.f * m_Scale, 190.f * m_Scale },
	m_Velocity				{ 395.f * m_Scale, 195.f * m_Scale },
	m_Acceleration			{ 0.f * m_Scale, -1000.f * m_Scale },

	// sounds
	m_HasPlayedSound		{ false }
{
	// texture data
	if (m_pGrenadeTexture == nullptr) {
		m_pGrenadeTexture = new Texture("Textures/PlasmaGrenade.png");
	}
	if (m_pExplosionTexture == nullptr) {
		m_pExplosionTexture = new Texture("Textures/PlasmaExplosion.png");
	}
	if(m_pExpolsionEffect == nullptr)
	{
		m_pExpolsionEffect = new SoundEffect("Sounds/plasmaGrenadeEffect.ogg");
	}
	++instanceCounter;

	// grenade data
	m_Shape.left	= m_StartPos.x;
	m_Shape.bottom	= m_StartPos.y;
	m_Shape.width	= m_pGrenadeTexture->GetWidth() * m_Scale;
	m_Shape.height	= m_pGrenadeTexture->GetHeight() * m_Scale;
}

PlasmaGrenade::~PlasmaGrenade()
{
	--instanceCounter;
	if (instanceCounter < 1) {
		delete m_pGrenadeTexture;
		m_pGrenadeTexture = nullptr;

		delete m_pExplosionTexture;
		m_pExplosionTexture = nullptr;

		delete m_pExpolsionEffect;
		m_pExpolsionEffect = nullptr;
	}
}

void PlasmaGrenade::Update(float elapsedSec, Avatar*& avatar, std::vector<BaseEnemy*>& enemies, Level& level)
{
	if (m_IsActive)
	{
		UpdateExpolsion(elapsedSec);

		if (!m_IsExplodiong)
		{

			// level colision
			level.HandleCollision(m_Shape, m_Velocity);
			m_IsOnGround = level.IsOnGround(m_Shape);

			// bounce
			if (m_IsOnGround)
			{
				m_HasHitGround = true;

				m_Velocity = m_BounceVelocity;
				m_BounceVelocity *= BOUNCE_FACTOR;
			}

			// movement
			m_Velocity += m_Acceleration * elapsedSec;
			m_Shape.left += m_Velocity.x * elapsedSec;
			m_Shape.bottom += m_Velocity.y * elapsedSec;
			if (m_IsLookingLeft)
			{
				m_Shape.left -= 2 * (m_Velocity.x * elapsedSec);
			}
		}
		else
		{
			CheckHit(enemies);
			CheckHit(avatar);

			if (!m_HasPlayedSound)
			{
				m_HasPlayedSound = true;
				m_pExpolsionEffect->Play(false);
			}
		}

		CheckAttach(enemies);
	}
}

void PlasmaGrenade::Draw() const
{
	if (m_IsActive)
	{
		if (!m_IsExplodiong)
		{
			m_pGrenadeTexture->Draw(m_Shape);
		}
		else
		{
			DrawExplosion();
		}
	}
}

void PlasmaGrenade::SetSoundVolume(int volume)
{
	m_pExpolsionEffect->SetVolume(volume);
}

void PlasmaGrenade::CleanUp()
{
	PlasmaGrenade::~PlasmaGrenade();
}

void PlasmaGrenade::UpdateExpolsion(float elapsedSec)
{
	// count to explosion
	m_ExplosionTime += elapsedSec;
	if (m_ExplosionTime > TIME_TO_EXPLODE)
	{
		m_IsExplodiong = true;

		m_AccuSec += elapsedSec;
		int nrFrames{ int(m_AccuSec / SECONDS_PER_FRAME) };
		m_CurrentFrame = nrFrames % NR_OF_EXPLOSION_FRAMES;
	}

	if (m_hasLoopedAnim and m_CurrentFrame == 0)
	{
		m_IsActive = false;
	}

	if (m_CurrentFrame == NR_OF_EXPLOSION_FRAMES - 1) {
		m_hasLoopedAnim = true;
	}
}

void PlasmaGrenade::DrawExplosion() const
{
	Rectf srcRect{};
	const float frameWidth{ m_pExplosionTexture->GetWidth() / NR_OF_EXPLOSION_FRAMES };
	const float fraweHeigth{ m_pExplosionTexture->GetHeight() };

	srcRect.left = frameWidth * m_CurrentFrame;
	srcRect.bottom = fraweHeigth;
	srcRect.width = frameWidth;
	srcRect.height = fraweHeigth;

	const float offsetY{ -3 };
	Rectf dstRect{
		(m_Shape.left + m_Shape.width / 2) - (frameWidth / 2) * m_Scale,
		m_Shape.bottom + offsetY * m_Scale,
		frameWidth * m_Scale,
		frameWidth * m_Scale
	};
	m_pExplosionTexture->Draw(dstRect, srcRect);
}

void PlasmaGrenade::CheckHit(std::vector<BaseEnemy*>& enemies)
{
	if (m_CurrentFrame == 1)
	{
		++m_CheckHitCounterEnemies;
		if (m_CheckHitCounterEnemies == 1)
		{
			m_CheckHitCounterEnemies = 2;

			Rectf greandeDamageRadius{
				(m_Shape.left + m_Shape.width / 2) - m_Radius,
				(m_Shape.bottom + m_Shape.height / 2) - m_Radius,
				m_Radius * 2,
				m_Radius * 2
			};
			for (int i{}; i < enemies.size(); ++i)
			{
				if (utils::IsOverlapping(enemies[i]->GetShape(), greandeDamageRadius))
				{
					enemies[i]->DecreaseHealth(m_GrenadeDamage);
				}
			}
		}
	}
}

void PlasmaGrenade::CheckHit(Avatar*& avatar)
{
	if (m_CurrentFrame == 1)
	{
		++m_CheckHitCounterAvatar;
		if (m_CheckHitCounterAvatar == 1)
		{
			m_CheckHitCounterAvatar = 2;

			Rectf grenadeDamageRadius{
				(m_Shape.left + m_Shape.width / 2) - m_Radius / 2,
				(m_Shape.bottom + m_Shape.height / 2) - m_Radius / 2,
				m_Radius * 2,
				m_Radius * 2
			};
			if (utils::IsOverlapping(avatar->GetShape(), grenadeDamageRadius))
			{
				avatar->DecreaseHealth(m_GrenadeDamage);
			}
		}
	}
}

void PlasmaGrenade::CheckAttach(std::vector<BaseEnemy*>& enemies)
{
	if (!m_HasHitGround)
	{
		for (int i{}; i < enemies.size(); ++i)
		{
			if(!m_IsAtached and !enemies[i]->GetIsDead() and utils::IsOverlapping(enemies[i]->GetShape(), m_Shape))
			{
				m_Velocity.x = 0;
				m_Velocity.y = 0;
				m_IsAtached = true;
				m_AtachedIdx = i;
			}
		}
	}
	if (m_IsAtached and !enemies[m_AtachedIdx]->GetIsDead())
	{
		float offsetX{ 10 };
		m_Shape.left = (enemies[m_AtachedIdx]->GetShape().left + enemies[m_AtachedIdx]->GetShape().width / 2) - offsetX * m_Scale;
		m_Shape.bottom = enemies[m_AtachedIdx]->GetShape().bottom + enemies[m_AtachedIdx]->GetShape().height / 2;
	}
}

void PlasmaGrenade::CheckAttach(Avatar*& avatar)
{
	if (!m_HasHitGround)
	{
		if (!avatar->GetIsDead() and utils::IsOverlapping(avatar->GetShape(), m_Shape))
		{
			float offsetX{ 10 };
			m_Shape.left = (avatar->GetShape().left + avatar->GetShape().width / 2) - offsetX * m_Scale;
			m_Shape.bottom = avatar->GetShape().bottom + avatar->GetShape().height / 2;
		}
	}
}
