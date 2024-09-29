#pragma once
#include "BaseEnemy.h"
#include "utils.h"
#include "Level.h"

class Neelder;

class Covenant_LittleRed: public BaseEnemy
{
public:
	Covenant_LittleRed(const Rectf& levelBoundaries, const Point2f& position, float scale);
	~Covenant_LittleRed();

	void Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, std::vector<BaseObject*>& objects, Level& level);
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
	const float		m_ShootingRadius;
	bool			m_HasReachedClosesPoint;
	bool			m_IsOnGround;
	Point2f			m_SavedPos;
	utils::HitInfo	m_Hitinfo;
	bool			m_IsLookingLeft;
	float			m_Health;
	int				m_ClosestMarineIdx;

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

	const float		m_SecPerFrame;

	// itemsDrops
	bool			m_HasDropedItems;

	void CheckDead();
	void CheckForMove(const Rectf& shape);
	void CheckReachedEdge(const Rectf& shape, Level& level);

	void DrawTexture() const;
	void UpdateTexture(float elapsedSec);

	void UpdateState();

	void CheckClosestPoint(const Rectf& shape);
	void HandleShooting();

	void CalcShootingAngle(const Rectf& shape);

	void CHeckForDrops(std::vector<BaseObject*>& objects);
};