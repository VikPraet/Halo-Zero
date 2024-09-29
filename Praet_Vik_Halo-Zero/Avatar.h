#pragma once
#include "SoundEffect.h"
#include "SoundEffect3.h"
#include <Vector2f.h>
#include "Texture.h"
#include "utils.h"
#include "Level.h"
#include <cmath>
#include "BaseWeapon.h"
#include "AutoRifle.h"
#include "BaseGrenade.h"
#include "FragGrenade.h"
#include "PlasmaGrenade.h"

class Avatar
{
public:
	Avatar(const Rectf& levelBoundaries, float scale = 1);
	~Avatar();

	void Update(float elapsedSec, Avatar*& avatar, const Point2f& mousePos, std::vector<BaseEnemy*>& enemies, Level& level);
	void Draw(const Point2f& mousePos) const;
	void DrawCroshair(const Point2f& mousePos) const;

	void AddHealth(float healthPoints);

	Rectf GetShape() const;
	std::vector<BaseBullet*> GetBullets() const;

	float GetMaxHealth() const;
	float GetHealth() const;
	float GetShield() const;

	bool GetIsDead() const;
	void DecreaseHealth(float hitpoints);

	void ThrowGrenade();
	void AddFragGrenade();
	void AddPlasmaGrenade();

	int GetNrOfFragGrenades() const;
	int GetNrOfPlasmaGrenades() const;
	int GetMaxNrOfPlasmaGrenades() const;

	BaseWeapon* GetWeapon() const;
	void SetWeapon(BaseWeapon*& weapon);

	int GetBulletsLeft() const;
	void CheckWeaponPickUp(Avatar*& avatar, std::vector<BaseEnemy*>& enemies);

	Vector2f GetVelocity() const;

	void SetVolume(int volume);

private:
	enum class LegsState {
		waiting = 1,
		running = 2,
		jumping = 3,
		staticCrouch = 4,
		crouchWalking = 5
	};

	// sounds
	SoundEffect* m_pDyingSound;
	SoundEffect* m_pNewWeaponSound;
	SoundEffect* m_pShieldLowSound;
	SoundEffect3* m_pShieldRegenSound;

	bool		 m_HasPlayedShieldLowSound;
	bool		 m_HasPlayedShieldRegenSound;
	bool		 m_HasStoppedShieldRegenSound;

	// health data
	float	m_MaxHealth;
	float	m_Health;
	float	m_MaxShield;
	float	m_Shield;
	float	m_ShieldRegenRate;

	float	m_ShieldRegenCooldown;
	float	m_AccuShieldTime;
	bool	m_IsShieldOnCooldown;
	bool	m_IsShieldRegen;
	bool	m_IsDead;

	// Weapon
	BaseWeapon* m_pWeapon;
	Rectf		m_LevelBoundaries;
	Point2f		m_BulletSpawnPos;
	float		groundToWeaponHeight;
	bool		m_FirstShot;

	// grenades
	int m_NrOfFragGrenades{};
	int m_NrOfPlasmaGrenades{};
	const int MAX_NR_OF_PLASMA_GRENADES;
	std::vector<BaseGrenade*> m_pGrenades;

	// Avatar data
	const float	m_Scale;
	Rectf		m_Shape;
	Rectf		m_LevelCollisionShape;
	LegsState	m_LegsState;
	bool		m_IsOnGround;
	bool		m_IsKeyPressed;
	bool		m_IsMousePressed;
	const float m_Crouchdistance;
	const float m_AvatarStandingHeight;

	// looking angle
	float		m_LookAngle;
	const float MAX_LOOK_ANGLE;
	bool		m_IsLookingBackwards;

	// speed
	const float HORIZONTAL_RUNNING_SPEED;
	const float HORIZONTAL_CROUCHING_SPEED;
	const float JUMP_SPEED;
	Vector2f	m_Velocity;
	Vector2f	m_Acceleration;

	//-----------------------------------------------------------------//
	// 
	// sprite data
	Texture*	m_pLegsSprite;
	Texture*	m_pThrowTexture;
	Texture*	m_pDeadTexture;
	const float SECONDS_PER_FRAME;
	Rectf		m_TorsoDstRect;
	Rectf		m_LegsDstRect;
	Rectf		m_DeadDstRect;

	// leg sprite
	const int	LEG_SPRITE_FRAMES_WAITING;
	const int	LEG_SPRITE_FRAMES_RUNNING;
	const int	LEG_SPRITE_FRAMES_JUMPING;
	const int	LEG_SPRITE_FRAMES_CROUCHING;
	const int	LEG_SPRITE_COLUMNS;
	const int	LEG_SPRITE_ROWS;
	int			m_CurrentLegFrame;
	float		m_AccuTimeLegs;

	// weapon tosro sprite
	const int	TORSO_SPRITE_SHOOTING_FRAMES;
	const int	TORSO_SPRITE_COLUMNS;
	const int	TORSO_SPRITE_ROWS;
	int			m_CurrentTorsoFrame;
	float		m_AccuTimeTorso;

	// grenade torso
	const int	TORSO_SPRITE_THROWING_FRAMES;
	const float SECONDS_PER_THROWING_FRAME;
	int			m_CurrentThrowingFrame;
	float		m_AccuTimeThrowing;
	bool		m_IsThrowingGrenade;
	int			m_CanThrowGrenadeCounter;

	// dead sprite data
	const int	DEAD_SPRITE_FRAMES;
	const int	DEAD_SPRITE_COLUMNS;
	const int	DEAD_SPRITE_ROWS;
	int			m_CurrentDeadFrame;
	float		m_AccuTimeDead;
	const float SEC_PER_DEAD_FRAME;
	bool		m_HasDeadAnimLooped;

	// functions
	void HandleMovement();
	void HandleLegs();

	void CalcLookAngle(const Point2f& mousePos);
	void CalcBulletSpawnPos(const Point2f& mousePos);

	void DrawLegsSprite() const;
	void UpdateLegsSprite(float elapsedSec, const Point2f& mousePos);

	void DrawTorsoSprite() const;
	void UpdateTorsoSprite(float elapsedSec, const Point2f& mousePos);

	void DrawThrowingSprite() const;
	void UpdateThrowingSprite(float elapsedSec);

	void DrawDeadSprite() const;
	void UpdateDeadSprite(float elapsedSec);

	void HandleShooting(const Point2f& mousePos);
	void CheckThrowGrenade();

	void handleShield(float elapsedSec);
	void CheckDead();

	void HandleStepHeight();

	void HandleShieldSounds();
};