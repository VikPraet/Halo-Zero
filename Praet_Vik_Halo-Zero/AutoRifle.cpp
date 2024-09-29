#include "pch.h"
#include "AutoRifle.h"
#include <iostream>
#include "Avatar.h"
Texture* AutoRifle::m_pStaticWeaponTexture{ nullptr };
SoundEffect* AutoRifle::m_pShotSound{ nullptr };
SoundEffect* AutoRifle::m_pReloadSound{ nullptr };
int AutoRifle::instanceCounter{ };

AutoRifle::AutoRifle(const Rectf& levelBoundaries, bool isHeldByPlayer, float scale) :
	BaseWeapon(levelBoundaries, isHeldByPlayer, scale),
	// sounds
	m_HasPlayedRelodingSound { false },

	// Texture Data
	m_pWeaponCroshair	{ new Texture("Textures/CrossHairAUTORIFLE.png")},
	m_pTorsoSprite		{ new Texture("Textures/AutoRifleTorso.png") },

	// Weapon Data
	m_FireRate			{ 625.f }, // RPM (rounds per minute)
	m_MagCapacity		{ 60 },
	m_TotalBullets		{ 60 },
	m_BulletsInMag		{ m_MagCapacity },
	m_TotBulletsLeft	{ m_TotalBullets },
	m_IsHeldByPlayer	{ isHeldByPlayer },
	m_AccuTimeShots		{ },

	// weapon data for enemie
	m_ShotsFired		{ },

	// static weapon data
	m_IsOnGround			{ false },
	m_StaticShape			{ },
	m_FallingAcceleration	{ 0.f, -1350.f * m_Scale },
	m_FallingVelocity		{ 0.f, 0.f },

	// reload data
	m_AccuTimeReloading	{ },
	m_ReloadingTime		{ 1.2f },
	m_IsReloading		{ false },
	m_ReloadCounter		{ },

	// bullet
	m_pBullets			{ },
	m_pDeletedBullets	{ },
	m_pBulletImpacts	{ },
	m_pBulletCasings	{ },

	// raycast
	m_HitInfo			{ }
{
	if (m_pStaticWeaponTexture == nullptr) {
		m_pStaticWeaponTexture = new Texture("Textures/AutoRifle.png");
	}
	if (m_pShotSound == nullptr) {
		m_pShotSound = new SoundEffect("Sounds/AutoRifleShot.ogg");
	}
	if (m_pReloadSound == nullptr) {
		m_pReloadSound = new SoundEffect("Sounds/Reload.ogg");
	}
	++instanceCounter;

	m_StaticShape.width = m_pStaticWeaponTexture->GetWidth() * m_Scale;
	m_StaticShape.height = m_pStaticWeaponTexture->GetHeight() * m_Scale;
}

AutoRifle::~AutoRifle()
{
	--instanceCounter;
	if (instanceCounter < 1)
	{
		delete m_pStaticWeaponTexture;
		m_pStaticWeaponTexture = nullptr;

		delete m_pShotSound;
		m_pShotSound = nullptr;

		delete m_pReloadSound;
		m_pReloadSound = nullptr;
	}

	delete m_pWeaponCroshair;
	m_pWeaponCroshair = nullptr;

	delete m_pTorsoSprite;
	m_pTorsoSprite = nullptr;

	ClearBullets();
}

void AutoRifle::CleanUp()
{
	AutoRifle::~AutoRifle();
}

void AutoRifle::Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, Level& level)
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

	for (BulletImpactYellow* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Update(elapsedSec);
	}

	for (BulletCasing* pBulletCasing : m_pBulletCasings)
	{
		pBulletCasing->Update(elapsedSec, level);
	}

	if (m_IsHeldByPlayer) {
		CheckForReload(elapsedSec);
	}

	CheckDeleteBullets(m_LevelBoundaries, level);
	CheckAvatarHit(avatar);
	CheckMarinesHit(marines);
}

void AutoRifle::Update(float elapsedSec, std::vector<BaseEnemy*>& enemies, Level& level)
{
	m_AccuTimeShots += elapsedSec;

	for (BaseBullet* pBullet : m_pBullets)
	{
		if (pBullet != nullptr)
		{
			pBullet->Update(elapsedSec);
		}
	}

	for (BulletImpactYellow* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Update(elapsedSec);
	}

	for (BulletCasing* pBulletCasing : m_pBulletCasings)
	{
		pBulletCasing->Update(elapsedSec, level);
	}

	if (m_IsHeldByPlayer) {
		CheckForReload(elapsedSec);
	}

	if (m_IsReloading and !m_HasPlayedRelodingSound)
	{
		m_pReloadSound->Play(false);
		m_HasPlayedRelodingSound = true;
	}
	else if(!m_IsReloading)
	{
		m_HasPlayedRelodingSound = false;
	}

	CheckDeleteBullets(m_LevelBoundaries, level);
	CheckEnemiesHit(enemies);
}

void AutoRifle::Draw() const
{
	DrawStaticWeapon();
	DrawBullets();
}

Texture* AutoRifle::GetTorsoSprite() const
{
	return m_pTorsoSprite;
}

void AutoRifle::DrawTorso(const Rectf& dstRect, const Rectf& srcRect) const
{
	m_pTorsoSprite->Draw(dstRect, srcRect);
}

void AutoRifle::DrawCroshair(const Point2f& mousepos) const
{
	Rectf dstRectCroshair{
		mousepos.x - (m_pWeaponCroshair->GetWidth() / 2) * m_Scale,
		mousepos.y - (m_pWeaponCroshair->GetHeight() / 2)* m_Scale,
		m_pWeaponCroshair->GetWidth() * m_Scale,
		m_pWeaponCroshair->GetHeight() * m_Scale
	};
	m_pWeaponCroshair->Draw(dstRectCroshair);
}

float AutoRifle::GetFireRate() const
{
	return m_FireRate;
}

void AutoRifle::DrawBullets() const
{
	for (BaseBullet* pBullet : m_pBullets)
	{
		if (pBullet != nullptr)
		{
			pBullet->Draw();
		}
	}

	for (BulletImpactYellow* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Draw();
	}

	for (BulletCasing* pBulletCasing : m_pBulletCasings)
	{
		pBulletCasing->Draw();
	}
}

void AutoRifle::UpdateStaticWeapon(float elapsedSec, Level& level)
{
	level.HandleCollision(m_StaticShape, m_FallingVelocity);

	m_FallingVelocity += m_FallingAcceleration * elapsedSec;
	m_StaticShape.left += m_FallingVelocity.x * elapsedSec;
	m_StaticShape.bottom += m_FallingVelocity.y * elapsedSec;
}

void AutoRifle::Shoot(const Point2f& bulletCenterPos, float angle, bool IsLookingBackwards)
{
	const Uint32 pStates = SDL_GetMouseState(nullptr, nullptr);
	const float secBetweenShost = 1 / (m_FireRate / 60);

	if (pStates == SDL_BUTTON(1) and m_IsHeldByPlayer)
	{
		if (m_AccuTimeShots > secBetweenShost and m_BulletsInMag > 0 and !m_IsReloading)
		{
			// spawn bullet
			m_pBullets.push_back(new AutoRifleBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
			--m_BulletsInMag;
			m_AccuTimeShots = 0;

			m_pShotSound->Play(false);

			// spawn casing
			float offset{ 35 };
			float angleThreshold{ 15 };
			Point2f CasingStartPos{ bulletCenterPos.x - offset * m_Scale, bulletCenterPos.y };
			if (angle > angleThreshold)
			{
				CasingStartPos.y -= offset;
			}
			else if (angle < -angleThreshold)
			{
				CasingStartPos.y += offset;
			}
			if (IsLookingBackwards)
			{
				CasingStartPos.x = bulletCenterPos.x + offset * m_Scale;
			}
			m_pBulletCasings.push_back(new BulletCasing(CasingStartPos, IsLookingBackwards, angle, m_Scale));
		}
	}
	else if (!m_IsHeldByPlayer)
	{
		if (m_AccuTimeShots > secBetweenShost)
		{
			m_pBullets.push_back(new AutoRifleBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
			m_AccuTimeShots = 0;
			++m_ShotsFired;

			m_pShotSound->Play(false);
		}
		if (m_ShotsFired == m_MagCapacity)
		{
			m_ShotsFired = 0;
			m_AccuTimeShots = -m_ReloadingTime;
			m_pReloadSound->Play(false);
		}
	}
}

void AutoRifle::CheckDeleteBullets(const Rectf& fieldBoundaries, Level& level)
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
			m_pBulletImpacts.push_back(new BulletImpactYellow(m_HitInfo.intersectPoint, m_Scale));

			m_pDeletedBullets.push_back(m_pBullets[i]);
			m_pBullets[i] = nullptr;
		}
		for (int j{}; j < level.GetWalls().size(); ++j)
		{
			if (m_pBullets[i] != nullptr and utils::Raycast(level.GetWallVert(j), Point2f{m_pBullets[i]->GetLeftMiddlePos().x,  m_pBullets[i]->GetLeftMiddlePos().y}, m_pBullets[i]->GetRightMiddlePos(), m_HitInfo))
			{
				m_pBulletImpacts.push_back(new BulletImpactYellow(m_HitInfo.intersectPoint, m_Scale));

				m_pDeletedBullets.push_back(m_pBullets[i]);
				m_pBullets[i] = nullptr;
			}
		}
	}
}

void AutoRifle::CheckForReload(float elapsedSec)
{
	const Uint8* pKeyboardState = SDL_GetKeyboardState(nullptr);
	const Uint32 pMouseState = SDL_GetMouseState(nullptr, nullptr);

	if (m_IsReloading)
	{
		m_AccuTimeReloading += elapsedSec;
	}

	if (pKeyboardState[SDL_SCANCODE_R] and pMouseState != SDL_BUTTON(1) and m_BulletsInMag < m_MagCapacity and m_TotBulletsLeft > 0)
	{
		m_IsReloading = true;
	}

	int nrOfBulletsToRefill{}, spaceInMag{};
	spaceInMag = m_MagCapacity - m_BulletsInMag;

	if (m_AccuTimeReloading > m_ReloadingTime)
	{
		if (spaceInMag < m_TotBulletsLeft)
		{
			nrOfBulletsToRefill = spaceInMag;
		}
		else
		{
			nrOfBulletsToRefill = m_TotBulletsLeft;
		}

		m_TotBulletsLeft -= nrOfBulletsToRefill;
		m_BulletsInMag += nrOfBulletsToRefill;

		m_AccuTimeReloading = 0;
		m_IsReloading = false;
	}
}

int AutoRifle::GetBulletsLeft() const
{
	if (!m_IsHeldByPlayer and m_AccuTimeShots < 0 or m_IsReloading)
	{
		return 0;
	}
	return m_BulletsInMag;
}

int AutoRifle::GetMaxBulletsInMag() const
{
	return m_MagCapacity;
}

int AutoRifle::GetTotBulletsLeft() const
{
	return m_TotBulletsLeft;
}

std::vector<BaseBullet*> AutoRifle::GetBullets() const
{
	return m_pBullets;
}

void AutoRifle::SetIsHeldByPlayer(bool isHeldByPlayer)
{
	m_IsHeldByPlayer = isHeldByPlayer;
}

void AutoRifle::DrawStaticWeapon() const
{
	if (m_IsOnGround)
	{
		m_pStaticWeaponTexture->Draw(m_StaticShape);
	}
}

void AutoRifle::SetStaticPosition(const Point2f& postition)
{
	m_StaticShape.left = postition.x;
	m_StaticShape.bottom = postition.y;
}

void AutoRifle::SetIsOnGround(bool isOnGround)
{
	m_IsOnGround = isOnGround;
}

Rectf AutoRifle::GetSaticShape() const
{
	return m_StaticShape;
}

float AutoRifle::GetCurrentCooldownTime() const
{
	return 0.0f;
}

float AutoRifle::GetMaxCooldownTime() const
{
	return 0.0f;
}

void AutoRifle::SetSFXVolume(int volume)
{
	m_pShotSound->SetVolume(volume);
	m_pReloadSound->SetVolume(volume);
}

void AutoRifle::CheckEnemiesHit(std::vector<BaseEnemy*>& enemies)
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

					m_pBulletImpacts.push_back(new BulletImpactYellow(m_pBullets[i]->GetCenterPos(), m_Scale));

					m_pDeletedBullets.push_back(m_pBullets[i]);
					m_pBullets[i] = nullptr;

				}
			}
		}
	}
}

void AutoRifle::CheckMarinesHit(std::vector<Marine*>& marines)
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

					m_pBulletImpacts.push_back(new BulletImpactYellow(m_pBullets[i]->GetCenterPos(), m_Scale));

					m_pDeletedBullets.push_back(m_pBullets[i]);
					m_pBullets[i] = nullptr;

				}
			}
		}
	}
}

void AutoRifle::CheckAvatarHit(Avatar*& avatar)
{
	if (!avatar->GetIsDead())
	{
		for (int i{}; i < m_pBullets.size(); ++i)
		{
			if (m_pBullets[i] != nullptr and utils::IsPointInRect(m_pBullets[i]->GetCenterPos(), avatar->GetShape()))
			{
				avatar->DecreaseHealth(m_pBullets[i]->GetBulletDamage());

				m_pBulletImpacts.push_back(new BulletImpactYellow(m_pBullets[i]->GetCenterPos(), m_Scale));

				m_pDeletedBullets.push_back(m_pBullets[i]);
				m_pBullets[i] = nullptr;

			}
		}
	}
}

void AutoRifle::ClearBullets()
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
	for (BulletImpactYellow* pBulletImpact : m_pBulletImpacts)
	{
		delete pBulletImpact;
		pBulletImpact = nullptr;
	}
	for (BulletCasing* pBulletCasing : m_pBulletCasings)
	{
		delete pBulletCasing;
		pBulletCasing = nullptr;
	}
	m_pBullets.clear();
	m_pDeletedBullets.clear();
	m_pBulletImpacts.clear();
	m_pBulletCasings.clear();
}