#include "pch.h"
#include "BigBlueGun.h"
#include "Avatar.h"

Texture* BigBlueGun::m_pStaticWeaponTexture{ nullptr };
SoundEffect* BigBlueGun::m_pShotSound{ nullptr };
int BigBlueGun::instanceCounter{ };

BigBlueGun::BigBlueGun(const Rectf& levelBoundaries, bool isHeldByPlayer, float scale) :
	BaseWeapon(levelBoundaries, isHeldByPlayer, scale),
	// Texture Data
	m_pWeaponCroshair		{ new Texture("Textures/CrossHairAUTORIFLE.png") },
	m_pTorsoSprite			{ new Texture("Textures/BigBlueRifleTorso.png") },

	// Weapon Data
	m_FireRate				{ 500.f }, // RPM
	m_IsHeldByPlayer		{ isHeldByPlayer },
	m_AccuTimeShots			{ },

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
		m_pStaticWeaponTexture = new Texture("Textures/BigBlueRifle.png");
	}

	if (m_pShotSound == nullptr) {
		m_pShotSound = new SoundEffect("Sounds/BigBlueShot.mp3");
	}
	++instanceCounter;

	m_StaticShape.width = m_pStaticWeaponTexture->GetWidth() * m_Scale;
	m_StaticShape.height = m_pStaticWeaponTexture->GetHeight() * m_Scale;
}

BigBlueGun::~BigBlueGun()
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

void BigBlueGun::CleanUp()
{
	BigBlueGun::~BigBlueGun();
}

void BigBlueGun::Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, Level& level)
{
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

	for (bullet_impact_BigBlue* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Update(elapsedSec);
	}

	CheckDeleteBullets(m_LevelBoundaries, level);
	CheckAvatarHit(avatar);
	CheckMarinesHit(marines);
}

void BigBlueGun::Update(float elapsedSec, std::vector<BaseEnemy*>& enemies, Level& level)
{
	m_AccuTimeShots += elapsedSec;

	for (BaseBullet* pBullet : m_pBullets)
	{
		if (pBullet != nullptr)
		{
			pBullet->Update(elapsedSec);
		}
	}

	for (bullet_impact_BigBlue* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Update(elapsedSec);
	}

	CheckDeleteBullets(m_LevelBoundaries, level);
	CheckEnemiesHit(enemies);
}

void BigBlueGun::Draw() const
{
	DrawStaticWeapon();
	DrawBullets();
}

Texture* BigBlueGun::GetTorsoSprite() const
{
	return m_pTorsoSprite;
}

void BigBlueGun::DrawTorso(const Rectf& dstRect, const Rectf& srcRect) const
{
	m_pTorsoSprite->Draw(dstRect, srcRect);
}

void BigBlueGun::DrawCroshair(const Point2f& mousepos) const
{
	Rectf dstRectCroshair{
		mousepos.x - (m_pWeaponCroshair->GetWidth() / 2) * m_Scale,
		mousepos.y - (m_pWeaponCroshair->GetHeight() / 2) * m_Scale,
		m_pWeaponCroshair->GetWidth() * m_Scale,
		m_pWeaponCroshair->GetHeight() * m_Scale
	};
	m_pWeaponCroshair->Draw(dstRectCroshair);
}

float BigBlueGun::GetFireRate() const
{
	return m_FireRate;
}

void BigBlueGun::DrawBullets() const
{
	for (BaseBullet* pBullet : m_pBullets)
	{
		if (pBullet != nullptr)
		{
			pBullet->Draw();
		}
	}

	for (bullet_impact_BigBlue* pBulletImpact : m_pBulletImpacts)
	{
		pBulletImpact->Draw();
	}
}

void BigBlueGun::UpdateStaticWeapon(float elapsedSec, Level& level)
{
	level.HandleCollision(m_StaticShape, m_FallingVelocity);

	m_FallingVelocity += m_FallingAcceleration * elapsedSec;
	m_StaticShape.left += m_FallingVelocity.x * elapsedSec;
	m_StaticShape.bottom += m_FallingVelocity.y * elapsedSec;
}

void BigBlueGun::Shoot(const Point2f& bulletCenterPos, float angle, bool IsLookingBackwards)
{
	if (m_IsHeldByPlayer)
	{
		const Uint32 pStates = SDL_GetMouseState(nullptr, nullptr);
		if (pStates == SDL_BUTTON(1))
		{
			const float secBetweenShost = 1 / (m_FireRate / 60);
			if (m_AccuTimeShots > secBetweenShost)
			{
				m_pBullets.push_back(new BigBlueBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
				m_AccuTimeShots = 0;
			}
		}
	}
	else
	{
		m_pBullets.push_back(new BigBlueBullet(bulletCenterPos, angle, IsLookingBackwards, m_Scale));
		m_pShotSound->Play(0);
	}
}

void BigBlueGun::CheckDeleteBullets(const Rectf& fieldBoundaries, Level& level)
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
			m_pBulletImpacts.push_back(new bullet_impact_BigBlue(m_HitInfo.intersectPoint, m_Scale));

			m_pDeletedBullets.push_back(m_pBullets[i]);
			m_pBullets[i] = nullptr;
		}
		for (int j{}; j < level.GetWalls().size(); ++j)
		{
			if (m_pBullets[i] != nullptr and utils::Raycast(level.GetWallVert(j), Point2f{ m_pBullets[i]->GetLeftMiddlePos().x,  m_pBullets[i]->GetLeftMiddlePos().y }, m_pBullets[i]->GetRightMiddlePos(), m_HitInfo))
			{
				m_pBulletImpacts.push_back(new bullet_impact_BigBlue(m_HitInfo.intersectPoint, m_Scale));

				m_pDeletedBullets.push_back(m_pBullets[i]);
				m_pBullets[i] = nullptr;
			}
		}
	}
}

int BigBlueGun::GetBulletsLeft() const
{
	return 1;
}

int BigBlueGun::GetMaxBulletsInMag() const
{
	return 0;
}

int BigBlueGun::GetTotBulletsLeft() const
{
	return 0;
}

std::vector<BaseBullet*> BigBlueGun::GetBullets() const
{
	return m_pBullets;
}

void BigBlueGun::SetIsHeldByPlayer(bool isHeldByPlayer)
{
	m_IsHeldByPlayer = isHeldByPlayer;
}

void BigBlueGun::DrawStaticWeapon() const
{
	if (m_IsOnGround)
	{
		m_pStaticWeaponTexture->Draw(m_StaticShape);
	}
}

void BigBlueGun::SetStaticPosition(const Point2f& postition)
{
	m_StaticShape.left = postition.x;
	m_StaticShape.bottom = postition.y;
}

void BigBlueGun::SetIsOnGround(bool isOnGround)
{
	m_IsOnGround = isOnGround;
}

Rectf BigBlueGun::GetSaticShape() const
{
	return m_StaticShape;
}

float BigBlueGun::GetCurrentCooldownTime() const
{
	return 0.0f;
}

float BigBlueGun::GetMaxCooldownTime() const
{
	return 0.0f;
}

void BigBlueGun::SetSFXVolume(int volume)
{
	m_pShotSound->SetVolume(volume);
}

void BigBlueGun::CheckEnemiesHit(std::vector<BaseEnemy*>& enemies)
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

					m_pBulletImpacts.push_back(new bullet_impact_BigBlue(m_pBullets[i]->GetCenterPos(), m_Scale));

					m_pDeletedBullets.push_back(m_pBullets[i]);
					m_pBullets[i] = nullptr;

				}
			}
		}
	}
}

void BigBlueGun::CheckMarinesHit(std::vector<Marine*>& marines)
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

					m_pBulletImpacts.push_back(new bullet_impact_BigBlue(m_pBullets[i]->GetCenterPos(), m_Scale));

					m_pDeletedBullets.push_back(m_pBullets[i]);
					m_pBullets[i] = nullptr;

				}
			}
		}
	}
}

void BigBlueGun::CheckAvatarHit(Avatar*& avatar)
{
	if (!avatar->GetIsDead())
	{
		for (int i{}; i < m_pBullets.size(); ++i)
		{
			if (m_pBullets[i] != nullptr and utils::IsPointInRect(m_pBullets[i]->GetCenterPos(), avatar->GetShape()))
			{
				avatar->DecreaseHealth(m_pBullets[i]->GetBulletDamage());

				m_pBulletImpacts.push_back(new bullet_impact_BigBlue(m_pBullets[i]->GetCenterPos(), m_Scale));

				m_pDeletedBullets.push_back(m_pBullets[i]);
				m_pBullets[i] = nullptr;

			}
		}
	}
}

void BigBlueGun::ClearBullets()
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
	for (bullet_impact_BigBlue* pBulletImpact : m_pBulletImpacts)
	{
		delete pBulletImpact;
		pBulletImpact = nullptr;
	}
	m_pBullets.clear();
	m_pDeletedBullets.clear();
	m_pBulletImpacts.clear();
}