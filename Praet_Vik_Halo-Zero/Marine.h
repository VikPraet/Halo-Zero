#pragma once
#include "utils.h"
#include "BaseEnemy.h"
#include "SoundEffect.h"
class Avatar;
class Level;

class Marine
{
public:
	Marine(const Rectf& levelBoundaries, const Point2f& position, float scale);
	~Marine();

	void Update(float elapsedSec, const Avatar& avatar, std::vector<BaseEnemy*>& enemies, std::vector<BaseObject*>& objects, Level& level);
	void Draw() const;
	void DrawWeapon() const;

	Rectf GetShape() const;
	bool GetIsDead() const;
	void DecreaseHealth(float hitpoints);

	BaseWeapon* GetWeapon() const;
	void SetWeapon(BaseWeapon*& weapon);

	void CleanUp();

	void SetSFXVolume(int volume);

private:
	// state data
	enum class State {
		waiting = 1,
		shooting = 2,
		running = 3,
		dead = 4
	};
	State m_State;
	// sounds
	static SoundEffect* m_pDyingSound;

	// static texture data
	static Texture* m_pTexture;
	static Texture* m_pTextureDead;
	static int		instanceCounter;

	// weapon data
	BaseWeapon*		m_pWeapon;
	float			m_ShootingAngle;
	float			m_MaxShootingAngle;
	int				m_FireRifleCounter;

	// enemy data
	const float		m_Scale;
	Rectf			m_Shape;
	Rectf			m_LevelCollisionShape;
	bool			m_IsDead;
	const float		m_HorizontalSpeed;
	Vector2f		m_Velocity;
	Vector2f		m_Acceleration;
	const float		m_MinActionRadius;
	const float		m_MaxActionRadius;
	const float		m_WalkingRadius;
	const float		m_ShootingRadius;
	bool			m_IsInShootingRange;
	bool			m_IsOnGround;
	Point2f			m_SavedPos;
	utils::HitInfo	m_Hitinfo;
	bool			m_IsLookingLeft;
	float			m_Health;

	int				m_ClosestEnemieIdx;

	// Sprite Data
	const int		m_SpriteCollumns;
	const int		m_SpriteRows;
	const int		m_NrWaitningFrames;
	const int		m_NrRunningFrames;
	const int		m_NrShootingFrames;
	int				m_CurrentFrame;
	float			m_AccuTime;

	const int		m_DeadSpriteCollumns;
	const int		m_DeadSpriteRows;
	const int		m_NrDeadFrames;
	bool			m_HasDeadAnimLooped;

	const float		m_SecPerFrameRunning;
	const float		m_SecPerFrameWalking;
	float			m_SecPerFrame;

	// itemsDrops
	bool			m_HasDropedItems;

	void CheckDead();
	void CheckForMove(const Point2f& avatarPosition, const std::vector<BaseEnemy*>& enemies);

	void DrawTexture() const;
	void UpdateTexture(float elapsedSec);

	void UpdateState();

	void HandleShooting();

	void CheckClosestEnemy(const std::vector<BaseEnemy*>& enemies);
	void CalcShootingAngle(const std::vector<BaseEnemy*>& enemies);

	void CHeckForDrops(const std::vector<BaseObject*>& objects);
	void CheckIsInShootingRange(const std::vector<BaseEnemy*>& enemies);

	void CheckFollow();
};

