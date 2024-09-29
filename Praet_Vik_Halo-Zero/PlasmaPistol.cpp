#include "pch.h"
#include "PlasmaPistol.h"
#include <iostream>
#include "Avatar.h"

Texture* PlasmaPistol::m_pStaticWeaponTexture{ nullptr };
SoundEffect* PlasmaPistol::m_pShotSound{ nullptr };
SoundEffect2* PlasmaPistol::m_pChargingSound{ nullptr };
int PlasmaPistol::instanceCounter{ };

PlasmaPistol::PlasmaPistol(const Rectf& levelBoundaries, bool isHeldByPlayer, float scale):
	BaseWeapon(levelBoundaries, isHeldByPlayer, scale),
	// sounds
	m_HasPlayedChargingSound		{ false },

	// Texture Data
	m_pWeaponCroshair		{ new Texture("Textures/CrossHairPLASMAPISTOL.png") },
	m_pTorsoSprite			{ new Texture("Textures/PlasmaPistolTorso.png") },

	// Weapon Data
	m_AnimSpeed				{ 500.f },
	m_TotalBullets			{ 85 },
	m_TotBulletsLeft		{ m_TotalBullets },
	m_IsHeldByPlayer		{ isHeldByPlayer },

	m_IsDoneCharging		{ false },
	m_AccuChargeTime		{ },
	m_MaxChargeTime			{ 1.1f },

	m_AccuCooldownTime		{ },
	m_LargeCooldownTime		{ 0.43f },
	m_NormalCooldownTime	{ 0.38f },
	
	m_OverheatCooldownTime	{ 2.8f },
	m_OverHeatTime			{ },
	m_OverHeatRate			{ 0.8f },
	m_IsOnOverHeatCooldown	{ false },

	m_IsOnShootCooldown		{ false },
	m_IsLargeBullet			{ false },
	m_CanShoot				{ false },
	m_HasFired				{ false },

	m_IsMousePressed		{ false },
	m_WasMousePressed		{ false },

	// static weapon data
	m_IsOnGround			{ false },
	m_StaticShape			{ },
	m_FallingAcceleration	{ 0.f, -1350.f * m_Scale },
	m_FallingVelocity		{ 0.f, 0.f },

	// bullet
	m_pBullets				{ },
	m_pDeletedBullets		{ },
	m_pBulletImpacts		{ },

	// enemy weapon data
	m_AccuEnemyShotsTime	{ },

	// raycast
	m_HitInfo				{ }
{
	if (m_pStaticWeaponTexture == nullptr) {
		m_pStaticWeaponTexture = new Texture("Textures/PlasmaPistol.png");
	}
	if (m_pShotSound == nullptr) {
		m_pShotSound = new SoundEffect("Sounds/PlasmaPistolShot.ogg");
	}
	if (m_pChargingSound == nullptr) {
		m_pChargingSound = new SoundEffect2("Sounds/PlasmaPistolCharge.ogg");
	}
	++instanceCounter;

	m_StaticShape.width = m_pStaticWeaponTexture->GetWidth() * m_Scale;
	m_StaticShape.height = m_pStaticWeaponTexture->GetHeight() * m_Scale;
}

PlasmaPistol::~PlasmaPistol()
{
	--instanceCounter;
	if (instanceCounter < 1)
	{
		delete m_pStaticWeaponTexture;
		m_pStaticWeaponTexture = nullptr;

		delete m_pShotSound;
		m_pShotSound = nullptr;

	}

	delete m_pChargingSound;
	m_pChargingSound = nullptr;

	delete m_pWeaponCroshair;
	m_pWeaponCroshair = nullptr;

	delete m_pTorsoSprite;
	m_pTorsoSprite = nullptr;

	ClearBullets();
}

void PlasmaPistol::CleanUp()
{
	PlasmaPistol::~PlasmaPistol();
}

void PlasmaPistol::Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, Level& level)
{
	m_AccuEnemyShotsTime += elapsedSec;

	if (m_IsOnGround)
	{
		UpdateStaticWeapon(elapsedSec, level);
	}

	for (int i{}; i < m_pBullets.size(); ++i)
	{
		if (m_pBullets[i] != nullptr)
		{
			m_pBullets[i]->Update(elapsedSec);
		}
	}

	for (BulletImpactGreen* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Update(elapsedSec);
	}

	if (m_IsHeldByPlayer)
	{
		CHeckCanShoot(elapsedSec);
		CheckLargeBullet(elapsedSec);
	}
	CheckDeleteBullets(m_LevelBoundaries, level);
	CheckOverHeat(elapsedSec);
	CheckAvatarHit(avatar);
	CheckMarinesHit(marines);
}

void PlasmaPistol::Update(float elapsedSec, std::vector<BaseEnemy*>& enemies, Level& level)
{
	for (int i{}; i < m_pBullets.size(); ++i)
	{
		if (m_pBullets[i] != nullptr)
		{
			m_pBullets[i]->Update(elapsedSec);
		}
	}

	for (BulletImpactGreen* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Update(elapsedSec);
	}

	CheckLargeBullet(elapsedSec);
	CheckOverHeat(elapsedSec);
	CHeckCanShoot(elapsedSec);
	CheckDeleteBullets(m_LevelBoundaries, level);
	CheckEnemiesHit(enemies);

	if (m_HasPlayedChargingSound and m_IsDoneCharging)
	{
		m_pChargingSound->StopAll();
		m_HasPlayedChargingSound = false;
	}
	if (m_CanShoot)
	{
		m_pShotSound->Play(false);
	}
}

void PlasmaPistol::Draw() const
{
	DrawStaticWeapon();
	DrawBullets();
}

void PlasmaPistol::DrawTorso(const Rectf& dstRect, const Rectf& srcRect) const
{
	m_pTorsoSprite->Draw(dstRect, srcRect);
}

void PlasmaPistol::DrawCroshair(const Point2f& mousepos) const
{
	Rectf dstRectCroshair{
		mousepos.x - (m_pWeaponCroshair->GetWidth() / 2) * m_Scale,
		mousepos.y - (m_pWeaponCroshair->GetHeight() / 2) * m_Scale,
		m_pWeaponCroshair->GetWidth() * m_Scale,
		m_pWeaponCroshair->GetHeight() * m_Scale
	};
	m_pWeaponCroshair->Draw(dstRectCroshair);
}

void PlasmaPistol::Shoot(const Point2f& bulletCenterPos, float angle, bool IsLookingBackwards)
{
	if (m_IsHeldByPlayer)
	{
		if (m_TotBulletsLeft > 0 and m_CanShoot)
		{
			if (m_IsLargeBullet)
			{
				m_pBullets.push_back(new StrongPlasmaPistolBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
			}
			else
			{
				m_pBullets.push_back(new PlasmaPistolBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
			}
			--m_TotBulletsLeft;

			m_WasMousePressed = false;
			m_IsOnShootCooldown = true;
		}
	}
	else
	{
		const float delayModifier{ 2.5f };
		const float secBetweenShost = m_NormalCooldownTime * delayModifier;
		if (m_AccuEnemyShotsTime > secBetweenShost)
		{
			m_pBullets.push_back(new PlasmaPistolBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
			m_AccuEnemyShotsTime = 0;
			--m_TotBulletsLeft;

			m_pShotSound->Play(false);

			int minimumBullets{ 25 };
			if (m_TotBulletsLeft < minimumBullets)
			{
				m_TotBulletsLeft = minimumBullets;
			}
		}
	}
	
}

Texture* PlasmaPistol::GetTorsoSprite() const
{
	return m_pTorsoSprite;
}

float PlasmaPistol::GetFireRate() const
{
	return m_AnimSpeed;
}

int PlasmaPistol::GetBulletsLeft() const
{
	if (m_IsOnShootCooldown or m_IsOnOverHeatCooldown)
	{
		return 0;
	}
	return m_TotBulletsLeft;
}

int PlasmaPistol::GetMaxBulletsInMag() const
{
	return m_TotalBullets;
}

int PlasmaPistol::GetTotBulletsLeft() const
{
	return m_TotBulletsLeft;
}

std::vector<BaseBullet*> PlasmaPistol::GetBullets() const
{
	return m_pBullets;
}

void PlasmaPistol::SetIsHeldByPlayer(bool isHeldByPlayer)
{
	m_IsHeldByPlayer = isHeldByPlayer;
}

void PlasmaPistol::DrawStaticWeapon() const
{
	if (m_IsOnGround)
	{
		m_pStaticWeaponTexture->Draw(m_StaticShape);
	}
}

void PlasmaPistol::SetStaticPosition(const Point2f& postition)
{
	m_StaticShape.left = postition.x;
	m_StaticShape.bottom = postition.y;
}

void PlasmaPistol::SetIsOnGround(bool isOnGround)
{
	m_IsOnGround = isOnGround;
}

Rectf PlasmaPistol::GetSaticShape() const
{
	return m_StaticShape;
}

float PlasmaPistol::GetCurrentCooldownTime() const
{
	return m_OverHeatTime;
}

float PlasmaPistol::GetMaxCooldownTime() const
{
	return m_OverheatCooldownTime;
}

void PlasmaPistol::SetSFXVolume(int volume)
{
	m_pShotSound->SetVolume(volume);
	m_pChargingSound->SetVolume(volume);
}

void PlasmaPistol::DrawBullets() const
{
	for (int i{}; i < m_pBullets.size(); ++i)
	{
		if (m_pBullets[i] != nullptr)
		{
			m_pBullets[i]->Draw();
		}
	}

	for (BulletImpactGreen* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Draw();
	}
}

void PlasmaPistol::UpdateStaticWeapon(float elapsedSec, Level& level)
{
	level.HandleCollision(m_StaticShape, m_FallingVelocity);

	m_FallingVelocity += m_FallingAcceleration * elapsedSec;
	m_StaticShape.left += m_FallingVelocity.x * elapsedSec;
	m_StaticShape.bottom += m_FallingVelocity.y * elapsedSec;
}

void PlasmaPistol::CheckDeleteBullets(const Rectf& fieldBoundaries, Level& level)
{
	for (int i{}; i < m_pBullets.size(); ++i)
	{
		if (m_pBullets[i] != nullptr and !utils::IsPointInRect(m_pBullets[i]->GetCenterPos(), fieldBoundaries))
		{
			m_pDeletedBullets.push_back(m_pBullets[i]);
			m_pBullets[i] = nullptr;
		}
		if (m_pBullets[i] != nullptr and utils::Raycast(level.GetVertices()[0], m_pBullets[i]->GetLeftMiddlePos(), m_pBullets[i]->GetRightMiddlePos(), m_HitInfo))
		{
			m_pBulletImpacts.push_back(new BulletImpactGreen(m_HitInfo.intersectPoint, m_Scale));

			m_pDeletedBullets.push_back(m_pBullets[i]);
			m_pBullets[i] = nullptr;
		}
		for (int j{}; j < level.GetWalls().size(); ++j)
		{
			if (m_pBullets[i] != nullptr and utils::Raycast(level.GetWallVert(j), Point2f{ m_pBullets[i]->GetLeftMiddlePos().x,  m_pBullets[i]->GetLeftMiddlePos().y }, m_pBullets[i]->GetRightMiddlePos(), m_HitInfo))
			{
				m_pBulletImpacts.push_back(new BulletImpactGreen(m_HitInfo.intersectPoint, m_Scale));

				m_pDeletedBullets.push_back(m_pBullets[i]);
				m_pBullets[i] = nullptr;
			}
		}
	}
}

void PlasmaPistol::CheckOverHeat(float elapsedSec)
{
	m_OverHeatTime -= elapsedSec;
	if(m_OverHeatTime < 0)
	{
		m_OverHeatTime = 0;
		m_IsOnOverHeatCooldown = false;
	}

	if (m_CanShoot)
	{
		if (m_IsLargeBullet)
		{
			m_OverHeatTime += m_OverheatCooldownTime;
		}
		else
		{
			m_OverHeatTime += m_OverHeatRate;
		}
	}

	if (m_OverHeatTime >= m_OverheatCooldownTime)
	{
		m_OverHeatTime = m_OverheatCooldownTime;
		m_IsOnOverHeatCooldown = true;
	}
}

void PlasmaPistol::CHeckCanShoot(float elapsedSec)
{
	const Uint32 pMouseState = SDL_GetMouseState(nullptr, nullptr);


	if (m_IsDoneCharging and pMouseState != SDL_BUTTON(1))
	{
		m_IsMousePressed = true;
	}
	else
	{
		if (m_IsMousePressed)
		{
			m_WasMousePressed = true;
		}
		m_IsMousePressed = false;
	}

	if (m_IsOnOverHeatCooldown)
	{
		m_WasMousePressed = false;
	}

	// check for cooldown
	if (m_IsOnShootCooldown)
	{
		m_AccuCooldownTime += elapsedSec;

		// check which cooldown
		if (m_IsLargeBullet)
		{
			if (m_AccuCooldownTime >= m_LargeCooldownTime)
			{
				m_IsOnShootCooldown = false;
				m_AccuCooldownTime = 0;
				m_IsLargeBullet = false;
			}
		}
		else
		{
			if (m_AccuCooldownTime >= m_NormalCooldownTime)
			{
				m_IsOnShootCooldown = false;
				m_AccuCooldownTime = 0;
			}
		}
	}

	// check if can shoot
	if (m_WasMousePressed and m_IsDoneCharging and !m_IsOnShootCooldown and !m_IsOnOverHeatCooldown)
	{
		m_CanShoot = true;
	}
	else
	{
		m_CanShoot = false;
	}
}

void PlasmaPistol::CheckLargeBullet(float elapsedSec)
{
	const Uint32 pMouseState = SDL_GetMouseState(nullptr, nullptr);

	if (pMouseState == SDL_BUTTON(1) and !m_IsOnOverHeatCooldown)
	{
		m_IsDoneCharging = false;
		m_AccuChargeTime += elapsedSec;
		if (!m_HasPlayedChargingSound)
		{
			m_pChargingSound->Play(false);
			m_HasPlayedChargingSound = true;
		}
	}
	else
	{
		m_IsDoneCharging = true;
		m_AccuChargeTime = 0;
	}

	if (m_AccuChargeTime >= m_MaxChargeTime)
	{
		m_AccuChargeTime = m_MaxChargeTime;
		m_IsLargeBullet = true;
	}
}

void PlasmaPistol::CheckEnemiesHit(std::vector<BaseEnemy*>& enemies)
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

					m_pBulletImpacts.push_back(new BulletImpactGreen(m_pBullets[i]->GetCenterPos(), m_Scale));

					m_pDeletedBullets.push_back(m_pBullets[i]);
					m_pBullets[i] = nullptr;
				}
			}
		}
	}
}

void PlasmaPistol::CheckMarinesHit(std::vector<Marine*>& marines)
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

					m_pBulletImpacts.push_back(new BulletImpactGreen(m_pBullets[i]->GetCenterPos(), m_Scale));

					m_pDeletedBullets.push_back(m_pBullets[i]);
					m_pBullets[i] = nullptr;
				}
			}
		}
	}
}

void PlasmaPistol::CheckAvatarHit(Avatar*& avatar)
{
	if (!avatar->GetIsDead())
	{
		for (int i{}; i < m_pBullets.size(); ++i)
		{
			if (m_pBullets[i] != nullptr and utils::IsPointInRect(m_pBullets[i]->GetCenterPos(), avatar->GetShape()))
			{
				avatar->DecreaseHealth(m_pBullets[i]->GetBulletDamage());

				m_pBulletImpacts.push_back(new BulletImpactGreen(m_pBullets[i]->GetCenterPos(), m_Scale));

				m_pDeletedBullets.push_back(m_pBullets[i]);
				m_pBullets[i] = nullptr;

			}
		}
	}
}

void PlasmaPistol::ClearBullets()
{

	for (int i{}; i < m_pBullets.size(); ++ i)
	{
		if (m_pBullets[i] != nullptr)
		{
			m_pBullets[i]->CleanUp();
		}

		delete m_pBullets[i];
		m_pBullets[i] = nullptr;
	}
	for (int i{}; i < m_pDeletedBullets.size(); ++i)
	{
		m_pDeletedBullets[i]->CleanUp();

		delete m_pDeletedBullets[i];
		m_pDeletedBullets[i] = nullptr;
	}

	for (int i{}; i < m_pBulletImpacts.size(); ++i)
	{
		delete m_pBulletImpacts[i];
		m_pBulletImpacts[i] = nullptr;
	}

	m_pBullets.clear();
	m_pDeletedBullets.clear();
	m_pBulletImpacts.clear();
}