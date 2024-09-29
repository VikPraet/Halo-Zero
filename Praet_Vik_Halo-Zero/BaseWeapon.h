#pragma once
#include "SoundEffect.h"
#include "Texture.h"
#include "Level.h"
#include "BaseBullet.h"
#include "BaseEnemy.h"

class Avatar;
class Marine;
class BaseWeapon
{
public:
	BaseWeapon(const Rectf& levelBoundaries, bool isHeldByPlayer , float scale);
	~BaseWeapon();

	virtual void CleanUp() = 0;

	virtual void Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, Level& level) = 0;
	virtual void Update(float elapsedSec, std::vector<BaseEnemy*>& enemies, Level& level) = 0;
	virtual void Draw() const = 0;

	virtual void DrawTorso(const Rectf& dstRect, const Rectf& srcRect) const = 0;
	virtual void DrawCroshair(const Point2f& mousepos) const = 0;

	virtual void Shoot(const Point2f& centerPos, float angle, bool IsLookingBackwards) = 0;

	virtual Texture* GetTorsoSprite() const = 0;
	virtual float GetFireRate() const = 0;
	virtual int GetBulletsLeft() const = 0;
	virtual int GetMaxBulletsInMag() const = 0;
	virtual int GetTotBulletsLeft() const = 0;
	virtual std::vector<BaseBullet*> GetBullets() const = 0;

	virtual void SetIsHeldByPlayer(bool isHeldByPlayer) = 0;

	virtual void DrawStaticWeapon() const = 0;

	virtual void SetStaticPosition(const Point2f& postition) = 0;
	virtual void SetIsOnGround(bool isOnGround) = 0;

	virtual Rectf GetSaticShape() const = 0;

	virtual float GetCurrentCooldownTime() const = 0;
	virtual float GetMaxCooldownTime() const = 0;

	virtual void SetSFXVolume(int volume) = 0;

protected:
	Rectf m_LevelBoundaries;
	bool m_IsHeldByPlayer;
	float m_Scale;
};

