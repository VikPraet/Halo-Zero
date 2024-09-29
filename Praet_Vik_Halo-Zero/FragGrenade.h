#pragma once
#include "BaseGrenade.h"
#include "utils.h"

class FragGrenade: public BaseGrenade
{
public:
	FragGrenade(const Point2f& starPos, bool isLookingLeft, float scale);
	~FragGrenade();

	void Update(float elapsedSec, Avatar*& avatar, std::vector<BaseEnemy*>& enemies, Level& level);
	void Draw() const;

	void SetSoundVolume(int volume);

	void CleanUp();

private:
	// texture data
	static Texture* m_pGrenadeTexture;
	static Texture* m_pExplosionTexture;
	static int		instanceCounter;

	// animation data
	bool		m_IsActive;
	bool		m_hasLoopedAnim;
	int			m_CurrentFrame;
	float		m_AccuSec;
	const int	NR_OF_EXPLOSION_FRAMES;
	const float SECONDS_PER_FRAME;

	// grenade data
	Rectf		m_Shape;
	bool		m_IsOnGround;
	const float BOUNCE_FACTOR;

	const float TIME_TO_EXPLODE;
	float		m_ExplosionTime;
	bool		m_IsExplodiong;

	// damage data
	int		m_CheckHitCounterEnemies;
	int		m_CheckHitCounterAvatar;
	float	m_GrenadeDamage;
	float	m_Radius;

	// speed
	Vector2f m_BounceVelocity;
	Vector2f m_Velocity;
	Vector2f m_Acceleration;

	// sounds
	bool				m_HasPlayedSound;
	static SoundEffect* m_pExpolsionEffect;

	void UpdateExpolsion(float elapsedSec);
	void DrawExplosion() const;

	void CheckHit(std::vector<BaseEnemy*>& enemies);
	void CheckHit(Avatar*& avatar);
};