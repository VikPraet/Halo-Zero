#include "pch.h"
#include "PlasmaRifle.h"
#include <iostream>
#include "Avatar.h"

Texture* PlasmaRifle::m_pStaticWeaponTexture{ nullptr };
SoundEffect* PlasmaRifle::m_pShotSound{ nullptr };
int PlasmaRifle::instanceCounter{ };

PlasmaRifle::PlasmaRifle(const Rectf& levelBoundaries, bool isHeldByPlayer, float scale) :
	BaseWeapon(levelBoundaries, isHeldByPlayer, scale),
	// Texture Data
	m_pWeaponCroshair		{ new Texture("Textures/CrossHairPLASMARIFLE.png") },
	m_pTorsoSprite			{ new Texture("Textures/PlasmaRifleTorso.png") },

	// Weapon Data
	m_FireRate				{ 345.f }, // RPM (rounds per minute)
	m_TotalBullets			{ 80 },
	m_TotBulletsLeft		{ m_TotalBullets },
	m_IsHeldByPlayer		{ isHeldByPlayer },
	m_AccuTimeShots			{ },
	m_HasShot				{ false },

	// cooldown
	m_IsOnCooldowm			{ false },
	m_MaxCooldownTime		{ 2.8f }, // seconds
	m_AccuCooldownTime		{ },
	m_CooldownIncreaseaRate	{ 0.4f },


	// static weapon data
	m_IsOnGround			{ false },
	m_StaticShape			{ },
	m_FallingAcceleration	{ 0.f, -1350.f * m_Scale },
	m_FallingVelocity		{ 0.f, 0.f },

	// bullet
	m_pBullets				{ },
	m_pDeletedBullets		{ },
	m_pBulletImpacts		{ },

	// raycast
	m_HitInfo{ }
{
	if (m_pStaticWeaponTexture == nullptr) {
		m_pStaticWeaponTexture = new Texture("Textures/PlasmaRifle.png");
	}
	if (m_pShotSound == nullptr) {
		m_pShotSound = new SoundEffect("Sounds/PlasmaRifleShot.mp3");
	}
	++instanceCounter;

	m_StaticShape.width = m_pStaticWeaponTexture->GetWidth() * m_Scale;
	m_StaticShape.height = m_pStaticWeaponTexture->GetHeight() * m_Scale;
}

PlasmaRifle::~PlasmaRifle()
{

	--instanceCounter;
	if (instanceCounter < 1)
	{
		delete m_pStaticWeaponTexture;
		m_pStaticWeaponTexture = nullptr;

		delete m_pShotSound;
		m_pShotSound = nullptr;
	}

	delete m_pWeaponCroshair;
	m_pWeaponCroshair = nullptr;

	delete m_pTorsoSprite;
	m_pTorsoSprite = nullptr;

	ClearBullets();
}

void PlasmaRifle::CleanUp()
{
	PlasmaRifle::~PlasmaRifle();
}

void PlasmaRifle::Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, Level& level)
{
	m_AccuTimeShots += elapsedSec;

	if (m_IsOnGround)
	{
		UpdateStaticWeapon(elapsedSec, level);
	}

	for (BaseBullet* pBullet : m_pBullets)
	{
		if (pBullet != nullptr)
		{
			pBullet->Update(elapsedSec);
		}
	}

	for (BulletImpactBlue* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Update(elapsedSec);
	}

	CheckCooldown(elapsedSec);
	CheckDeleteBullets(m_LevelBoundaries, level);
	CheckAvatarHit(avatar);
	CheckMarinesHit(marines);
}

void PlasmaRifle::Update(float elapsedSec, std::vector<BaseEnemy*>& enemies, Level& level)
{
	m_AccuTimeShots += elapsedSec;

	for (BaseBullet* pBullet : m_pBullets)
	{
		if (pBullet != nullptr)
		{
			pBullet->Update(elapsedSec);
		}
	}

	for (BulletImpactBlue* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Update(elapsedSec);
	}

	CheckCooldown(elapsedSec);
	CheckDeleteBullets(m_LevelBoundaries, level);
	CheckEnemiesHit(enemies);
}

void PlasmaRifle::Draw() const
{
	DrawStaticWeapon();
	DrawBullets();
}

Texture* PlasmaRifle::GetTorsoSprite() const
{
	return m_pTorsoSprite;
}

void PlasmaRifle::DrawTorso(const Rectf& dstRect, const Rectf& srcRect) const
{
	m_pTorsoSprite->Draw(dstRect, srcRect);
}

void PlasmaRifle::DrawCroshair(const Point2f& mousepos) const
{
	Rectf dstRectCroshair{
		mousepos.x - (m_pWeaponCroshair->GetWidth() / 2) * m_Scale,
		mousepos.y - (m_pWeaponCroshair->GetHeight() / 2) * m_Scale,
		m_pWeaponCroshair->GetWidth() * m_Scale,
		m_pWeaponCroshair->GetHeight() * m_Scale
	};
	m_pWeaponCroshair->Draw(dstRectCroshair);
}

float PlasmaRifle::GetFireRate() const
{
	return m_FireRate;
}

void PlasmaRifle::DrawBullets() const
{
	for (BaseBullet* pBullet : m_pBullets)
	{
		if (pBullet != nullptr)
		{
			pBullet->Draw();
		}
	}

	for (BulletImpactBlue* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Draw();
	}
}

void PlasmaRifle::UpdateStaticWeapon(float elapsedSec, Level& level)
{
	level.HandleCollision(m_StaticShape, m_FallingVelocity);

	m_FallingVelocity += m_FallingAcceleration * elapsedSec;
	m_StaticShape.left += m_FallingVelocity.x * elapsedSec;
	m_StaticShape.bottom += m_FallingVelocity.y * elapsedSec;
}

void PlasmaRifle::Shoot(const Point2f& bulletCenterPos, float angle, bool IsLookingBackwards)
{
	const float secBetweenShots{ 1 / (m_FireRate / 60) };
	if (m_IsHeldByPlayer)
	{
		const Uint32 pStates = SDL_GetMouseState(nullptr, nullptr);
		if (pStates == SDL_BUTTON(1))
		{
			if (m_AccuTimeShots > secBetweenShots and m_TotBulletsLeft > 0 and !m_IsOnCooldowm)
			{
				m_pBullets.push_back(new PlasmaRifleBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
				--m_TotBulletsLeft;
				m_AccuTimeShots = 0;
				m_HasShot = true;

				m_pShotSound->Play(0);
			}
		}
	}
	else
	{
		const float EnemyShotsDelay{0.07f};
		if (m_AccuTimeShots > (secBetweenShots + EnemyShotsDelay) and !m_IsOnCooldowm)
		{
			m_pBullets.push_back(new PlasmaRifleBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
			m_AccuTimeShots = 0;
			m_HasShot = true;

			m_pShotSound->Play(0);
		}
	}
}

void PlasmaRifle::CheckDeleteBullets(const Rectf& fieldBoundaries, Level& level)
{
	for (int i{}; i < m_pBullets.size(); ++i)
	{
		if (m_pBullets[i] != nullptr and !utils::IsPointInRect(m_pBullets[i]->GetCenterPos(), fieldBoundaries))
		{
			m_pDeletedBullets.push_back(m_pBullets[i]);
			m_pBullets[i] = nullptr;
		}
		if (m_pBullets[i] != nullptr and utils::Raycast(level.GetVertices()[0], Point2f{ m_pBullets[i]->GetLeftMiddlePos().x,  m_pBullets[i]->GetLeftMiddlePos().y }, m_pBullets[i]->GetRightMiddlePos(), m_HitInfo))
		{
			m_pBulletImpacts.push_back(new BulletImpactBlue(m_HitInfo.intersectPoint, m_Scale));

			m_pDeletedBullets.push_back(m_pBullets[i]);
			m_pBullets[i] = nullptr;
		}
		for (int j{}; j < level.GetWalls().size(); ++j)
		{
			if (m_pBullets[i] != nullptr and utils::Raycast(level.GetWallVert(j), Point2f{ m_pBullets[i]->GetLeftMiddlePos().x,  m_pBullets[i]->GetLeftMiddlePos().y }, m_pBullets[i]->GetRightMiddlePos(), m_HitInfo))
			{
				m_pBulletImpacts.push_back(new BulletImpactBlue(m_HitInfo.intersectPoint, m_Scale));

				m_pDeletedBullets.push_back(m_pBullets[i]);
				m_pBullets[i] = nullptr;
			}
		}
	}
}

void PlasmaRifle::CheckCooldown(float elapsedSec)
{
	m_AccuCooldownTime -= elapsedSec;
	if (m_AccuCooldownTime < 0)
	{
		m_AccuCooldownTime = 0;
		m_IsOnCooldowm = false;
	}
	if (m_HasShot)
	{
		m_AccuCooldownTime += m_CooldownIncreaseaRate;
		m_HasShot = false;
	}

	if (m_AccuCooldownTime >= m_MaxCooldownTime)
	{
		m_AccuCooldownTime = m_MaxCooldownTime;
		m_IsOnCooldowm = true;
	}
}

int PlasmaRifle::GetBulletsLeft() const
{
	if (m_IsOnCooldowm)
	{
		return 0;
	}
	return m_TotBulletsLeft;
}

int PlasmaRifle::GetMaxBulletsInMag() const
{
	return m_TotalBullets;
}

int PlasmaRifle::GetTotBulletsLeft() const
{
	return m_TotBulletsLeft;
}

std::vector<BaseBullet*> PlasmaRifle::GetBullets() const
{
	return m_pBullets;
}

void PlasmaRifle::SetIsHeldByPlayer(bool isHeldByPlayer)
{
	m_IsHeldByPlayer = isHeldByPlayer;
}

void PlasmaRifle::DrawStaticWeapon() const
{
	if (m_IsOnGround)
	{
		m_pStaticWeaponTexture->Draw(m_StaticShape);
	}
}

void PlasmaRifle::SetStaticPosition(const Point2f& postition)
{
	m_StaticShape.left = postition.x;
	m_StaticShape.bottom = postition.y;
}

void PlasmaRifle::SetIsOnGround(bool isOnGround)
{
	m_IsOnGround = isOnGround;
}

Rectf PlasmaRifle::GetSaticShape() const
{
	return m_StaticShape;
}

float PlasmaRifle::GetCurrentCooldownTime() const
{
	return m_AccuCooldownTime;
}

float PlasmaRifle::GetMaxCooldownTime() const
{
	return m_MaxCooldownTime;
}

void PlasmaRifle::SetSFXVolume(int volume)
{
	m_pShotSound->SetVolume(volume);
}

void PlasmaRifle::CheckEnemiesHit(std::vector<BaseEnemy*>& enemies)
{
	for (int j{}; j < enemies.size(); ++j)
	{
		if (!enemies[j]->GetIsDead())
		{
			for (int i{}; i < m_pBullets.size(); ++i)
			{
				if (m_pBullets[i] != nullptr and utils::IsPointInRect(m_pBullets[i]->GetCenterPos(), enemies[j]->GetShape()))
				{
					enemies[j]->DecreaseHealth(m_pBullets[i]->GetBulletDamage());

					m_pBulletImpacts.push_back(new BulletImpactBlue(m_pBullets[i]->GetCenterPos(), m_Scale));

					m_pDeletedBullets.push_back(m_pBullets[i]);
					m_pBullets[i] = nullptr;

				}
			}
		}
	}
}

void PlasmaRifle::CheckMarinesHit(std::vector<Marine*>& marines)
{
	for (int j{}; j < marines.size(); ++j)
	{
		if (!marines[j]->GetIsDead())
		{
			for (int i{}; i < m_pBullets.size(); ++i)
			{
				if (m_pBullets[i] != nullptr and utils::IsPointInRect(m_pBullets[i]->GetCenterPos(), marines[j]->GetShape()))
				{
					marines[j]->DecreaseHealth(m_pBullets[i]->GetBulletDamage());

					m_pBulletImpacts.push_back(new BulletImpactBlue(m_pBullets[i]->GetCenterPos(), m_Scale));

					m_pDeletedBullets.push_back(m_pBullets[i]);
					m_pBullets[i] = nullptr;

				}
			}
		}
	}
}

void PlasmaRifle::CheckAvatarHit(Avatar*& avatar)
{
	if (!avatar->GetIsDead())
	{
		for (int i{}; i < m_pBullets.size(); ++i)
		{
			if (m_pBullets[i] != nullptr and utils::IsPointInRect(m_pBullets[i]->GetCenterPos(), avatar->GetShape()))
			{
				avatar->DecreaseHealth(m_pBullets[i]->GetBulletDamage());

				m_pBulletImpacts.push_back(new BulletImpactBlue(m_pBullets[i]->GetCenterPos(), m_Scale));

				m_pDeletedBullets.push_back(m_pBullets[i]);
				m_pBullets[i] = nullptr;

			}
		}
	}
}

void PlasmaRifle::ClearBullets()
{
	for (BaseBullet* pBullet : m_pBullets)
	{
		if (pBullet != nullptr)
		{
			pBullet->CleanUp();
		}

		delete pBullet;
		pBullet = nullptr;
	}
	for (BaseBullet* pBullet : m_pDeletedBullets)
	{
		pBullet->CleanUp();

		delete pBullet;
		pBullet = nullptr;
	}
	for (BulletImpactBlue* pBulletImpact : m_pBulletImpacts)
	{
		delete pBulletImpact;
		pBulletImpact = nullptr;
	}
	m_pBullets.clear();
	m_pDeletedBullets.clear();
	m_pBulletImpacts.clear();
}