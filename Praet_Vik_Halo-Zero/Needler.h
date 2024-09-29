#pragma once
#include "BaseWeapon.h"
#include "Texture.h"
#include "BaseBullet.h"
#include "NeedlerBullet.h"
#include "BulletImpactPurple.h"
#include "Marine.h"

class Needler: public BaseWeapon
{
public:
	Needler(const Rectf& levelBoundaries, bool isHeldByPlayer, float scale);
	~Needler();

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
	static SoundEffect* m_pShotSound;
	static SoundEffect* m_pReloadSound;
	bool		 m_HasPlayedRelodingSound;

	// Texture Data
	static int		instanceCounter;
	static Texture* m_pStaticWeaponTexture;
	Texture*		m_pWeaponCroshair;
	Texture*		m_pTorsoSprite;

	// Weapon Data
	const float m_FireRate;
	const int	m_MagCapacity;
	const int	m_TotalBullets;
	int			m_BulletsInMag;
	int			m_TotBulletsLeft;
	bool		m_IsHeldByPlayer;
	float		m_AccuTimeShots;

	// weapon data for enemie
	int m_ShotsFired;

	// static weapon data
	bool		m_IsOnGround;
	Rectf		m_StaticShape;
	Vector2f	m_FallingAcceleration;
	Vector2f	m_FallingVelocity;

	// reload data
	float	m_AccuTimeReloading;
	float	m_ReloadingTime;
	bool	m_IsReloading;
	bool	m_ReloadCounter;

	// buttet
	std::vector<BaseBullet*>			m_pBullets;
	std::vector<BaseBullet*>			m_pDeletedBullets;
	std::vector<BulletImpactPurple*>	m_pBulletImpacts;

	// raycast
	utils::HitInfo	m_HitInfo;

	void ClearBullets();

	void DrawBullets() const;

	void UpdateStaticWeapon(float elapsedSec, Level& level);

	void CheckForReload(float elapsedSec);
	void CheckDeleteBullets(const Rectf& fieldBoundaries, Level& level);

	void CheckEnemiesHit(std::vector<BaseEnemy*>& enemies);
	void CheckMarinesHit(std::vector<Marine*>& marines);
	void CheckAvatarHit(Avatar*& avatar);
};
