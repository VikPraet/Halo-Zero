#pragma once
#include "BaseWeapon.h"
#include "Texture.h"
#include "BaseBullet.h"
#include "PlasmaPistolBullet.h"
#include "StrongPlasmaPistolBullet.h"
#include "BulletImpactGreen.h"
#include "Marine.h"
#include "SoundEffect2.h"

class PlasmaPistol: public BaseWeapon
{
public:
	PlasmaPistol(const Rectf& levelBoundaries, bool isHeldByPlayer, float scale);
	~PlasmaPistol();

	void CleanUp() override;

	void Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, Level& level) override;
	void Update(float elapsedSec, std::vector<BaseEnemy*>& enemies, Level& level) override;
	void Draw() const override;

	void DrawTorso(const Rectf& dstRect, const Rectf& srcRect) const override;
	void DrawCroshair(const Point2f& mousepos) const override;

	void Shoot(const Point2f& bulletCenterPos, float angle, bool IsLookingBackwards) override;

	Texture* GetTorsoSprite() const override;
	float GetFireRate() const override;
	int GetBulletsLeft() const override;
	int GetMaxBulletsInMag() const override;
	int GetTotBulletsLeft() const override;
	std::vector<BaseBullet*> GetBullets() const override;

	void SetIsHeldByPlayer(bool isHeldByPlayer);

	void DrawStaticWeapon() const;

	void SetStaticPosition(const Point2f& postition);
	void SetIsOnGround(bool isOnGround);

	Rectf GetSaticShape() const;

	float GetCurrentCooldownTime() const;
	float GetMaxCooldownTime() const;

	void SetSFXVolume(int volume) override;

private:
	// sounds
	static SoundEffect*		m_pShotSound;
	static SoundEffect2*	m_pChargingSound;
	bool					m_HasPlayedChargingSound;

	// Texture Data
	static int		instanceCounter;
	static Texture* m_pStaticWeaponTexture;
	Texture*		m_pWeaponCroshair;
	Texture*		m_pTorsoSprite;

	// Weapon Data
	const float m_AnimSpeed;
	const int	m_TotalBullets;
	int			m_TotBulletsLeft;
	bool		m_IsHeldByPlayer;

	bool		m_IsDoneCharging;
	float		m_AccuChargeTime;
	float		m_MaxChargeTime;

	float		m_AccuCooldownTime;
	float		m_LargeCooldownTime;
	float		m_NormalCooldownTime;

	float		m_OverheatCooldownTime;
	float		m_OverHeatTime;
	float		m_OverHeatRate;
	bool		m_IsOnOverHeatCooldown;

	bool		m_IsOnShootCooldown;
	bool		m_IsLargeBullet;
	bool		m_CanShoot;
	bool		m_HasFired;

	bool		m_IsMousePressed;
	bool		m_WasMousePressed;

	// static weapon data
	bool		m_IsOnGround;
	Rectf		m_StaticShape;
	Vector2f	m_FallingAcceleration;
	Vector2f	m_FallingVelocity;

	// enemy weapon data
	float		m_AccuEnemyShotsTime;

	// bullet
	std::vector<BaseBullet*>		m_pBullets;
	std::vector<BaseBullet*>		m_pDeletedBullets;
	std::vector<BulletImpactGreen*> m_pBulletImpacts;

	// raycast
	utils::HitInfo m_HitInfo;

	void ClearBullets();

	void DrawBullets() const;

	void UpdateStaticWeapon(float elapsedSec, Level& level);

	void CheckDeleteBullets(const Rectf& fieldBoundaries, Level& level);

	void CheckOverHeat(float elapsedSec);
	void CHeckCanShoot(float elapsedSec);
	void CheckLargeBullet(float elapsedSec);

	void CheckEnemiesHit(std::vector<BaseEnemy*>& enemies);
	void CheckMarinesHit(std::vector<Marine*>& marines);
	void CheckAvatarHit(Avatar*& avatar);
};