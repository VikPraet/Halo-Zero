#include "pch.h"
#include "Marine.h"
#include "Avatar.h"
#include "Level.h"
#include "AutoRifle.h"

// static texture data
Texture* Marine::m_pTexture{ nullptr };
Texture* Marine::m_pTextureDead{ nullptr };
SoundEffect* Marine::m_pDyingSound{ nullptr };
int Marine::instanceCounter{ };

Marine::Marine(const Rectf& levelBoundaries, const Point2f& position, float scale) :
	// state data
	m_State{ State::waiting },

	// weapon data
	m_pWeapon				{ new AutoRifle(levelBoundaries, false, scale) },
	m_ShootingAngle			{ },
	m_MaxShootingAngle		{ 20.f },
	m_FireRifleCounter		{ },

	// enemy data
	m_Scale					{ scale },
	m_Shape					{ position.x, position.y , 35.f, 100.f },
	m_LevelCollisionShape	{ },
	m_IsDead				{ false },
	m_HorizontalSpeed		{ 195.f * m_Scale },
	m_Velocity				{ 0.f, 0.f },
	m_Acceleration			{ 0.0f * m_Scale, -1350.f * m_Scale },
	m_MinActionRadius		{ 80.f * m_Scale },
	m_MaxActionRadius		{ 600.f * m_Scale },
	m_WalkingRadius			{ 120.f * m_Scale },
	m_ShootingRadius		{ 190.f * m_Scale },
	m_IsInShootingRange		{ false },
	m_IsOnGround			{ },
	m_IsLookingLeft			{ true },
	m_Health				{ 150 },
	m_ClosestEnemieIdx		{ },

	// sprite data
	m_SpriteCollumns		{ 7 },
	m_SpriteRows			{ 3 },
	m_NrWaitningFrames		{ 1 },
	m_NrRunningFrames		{ 6 },
	m_NrShootingFrames		{ 2 },
	m_CurrentFrame			{ },
	m_AccuTime				{ },

	m_DeadSpriteCollumns	{ 3 },
	m_DeadSpriteRows		{ 1 },
	m_NrDeadFrames			{ 3 },
	m_HasDeadAnimLooped		{ false },

	m_SecPerFrameRunning	{ 0.1f },
	m_SecPerFrameWalking	{ 0.14f },
	m_SecPerFrame			{ m_SecPerFrameRunning },

	// itemDrops
	m_HasDropedItems{ false }
{
	if (m_pTexture == nullptr) {
		m_pTexture = new Texture("Textures/Marine.png");
	}
	if (m_pTextureDead == nullptr) {
		m_pTextureDead = new Texture("Textures/Marine_Dead.png");
	}
	if (m_pDyingSound == nullptr) {
		m_pDyingSound = new SoundEffect("Sounds/MarineDeath.ogg");
	}
	++instanceCounter;

	m_Shape.left *= m_Scale;
	m_Shape.bottom *= m_Scale;
	m_Shape.width *= m_Scale;
	m_Shape.height *= m_Scale;

	m_LevelCollisionShape.width = 107 * m_Scale;
	m_LevelCollisionShape.height = m_Shape.height;
	m_LevelCollisionShape.bottom = m_Shape.bottom;
	m_LevelCollisionShape.left = m_Shape.left + m_Shape.width / 2 - m_LevelCollisionShape.width / 2;

	m_SavedPos.x = m_Shape.left;
	m_SavedPos.y = m_Shape.bottom;
}

Marine::~Marine()
{
	--instanceCounter;
	if (instanceCounter < 1)
	{
		delete m_pTexture;
		m_pTexture = nullptr;

		delete m_pTextureDead;
		m_pTextureDead = nullptr;

		delete m_pDyingSound;
		m_pDyingSound = nullptr;
	}

	m_pWeapon->CleanUp();

	delete m_pWeapon;
	m_pWeapon = nullptr;
}

void Marine::Update(float elapsedSec, const Avatar& avatar, std::vector<BaseEnemy*>& enemies, std::vector<BaseObject*>& objects, Level& level)
{
	CheckClosestEnemy(enemies);

	// do collision detection
	level.HandleCollision(m_LevelCollisionShape, m_Velocity);
	m_IsOnGround = level.IsOnGround(m_LevelCollisionShape);

	CheckForMove(Point2f{ avatar.GetShape().left, avatar.GetShape().bottom }, enemies);

	// check if enemy is dead
	CheckDead();

	// update shape position
	m_Shape.left = m_LevelCollisionShape.left + m_LevelCollisionShape.width / 2 - m_Shape.width / 2;
	m_Shape.bottom = m_LevelCollisionShape.bottom;

	// update position
	m_Velocity += m_Acceleration * elapsedSec;
	m_LevelCollisionShape.left += m_Velocity.x * elapsedSec;
	m_LevelCollisionShape.bottom += m_Velocity.y * elapsedSec;

	CheckIsInShootingRange(enemies);
	UpdateState();
	UpdateTexture(elapsedSec);

	// calculate angle for bullets
	if (m_State == State::shooting)
	{
		CalcShootingAngle(enemies);
	}

	if (!m_IsDead)
	{
		float distanceToEnemie{ abs(enemies[m_ClosestEnemieIdx]->GetShape().left - m_Shape.left) };
		if (m_State == State::shooting or distanceToEnemie < m_MaxActionRadius)
		{
			if (enemies[m_ClosestEnemieIdx]->GetShape().left > m_Shape.left)
			{
				m_IsLookingLeft = false;
			}
			else {
				m_IsLookingLeft = true;
			}
		}
		else
		{
			if (avatar.GetShape().left > m_Shape.left)
			{
				m_IsLookingLeft = false;
			}
			else
			{
				m_IsLookingLeft = true;
			}
		}

		HandleShooting();
	}

	CHeckForDrops(objects);
	m_pWeapon->Update(elapsedSec, enemies, level);
}

void Marine::Draw() const
{
	glPushMatrix();
	if (!m_IsLookingLeft) {
		glTranslatef(m_Shape.left * 2 + m_Shape.width, 0, 0);
		glScalef(-1, 1, 1);
	}
	DrawTexture();
	glPopMatrix();

	// draw hitbox
	//utils::SetColor(Color4f{ 0.9f, 1.f, 0.0f, 1.0f });
	//utils::DrawRect(m_Shape);
	//utils::SetColor(Color4f{ 0.5f, 1.f, 0.7f, 1.0f });
	//utils::DrawRect(m_LevelCollisionShape);
}

void Marine::DrawWeapon() const
{
	m_pWeapon->Draw();
}

Rectf Marine::GetShape() const
{
	return m_Shape;
}

bool Marine::GetIsDead() const
{
	return m_IsDead;
}

void Marine::CheckDead()
{
	if (m_Health <= 0)
	{
		if (!m_IsDead)
		{
			m_AccuTime = 0;
			m_pDyingSound->Play(0);
		}
		m_Health = 0;
		m_IsDead = true;
	}
}

void Marine::DecreaseHealth(float hitpoints)
{
	m_Health -= hitpoints;
}

BaseWeapon* Marine::GetWeapon() const
{
	return m_pWeapon;
}

void Marine::SetWeapon(BaseWeapon*& weapon)
{
	m_pWeapon = weapon;
	m_pWeapon->SetStaticPosition(Point2f{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom });
}

void Marine::CleanUp()
{
	Marine::~Marine();
}

void Marine::SetSFXVolume(int volume)
{
	m_pWeapon->SetSFXVolume(volume);
	m_pDyingSound->SetVolume(volume);
}

void Marine::CheckForMove(const Point2f& avatarPosition, const std::vector<BaseEnemy*>& enemies)
{
	float distanceToAvatar{ abs(avatarPosition.x - m_Shape.left) };
	float distanceToEnemie{ abs(enemies[m_ClosestEnemieIdx]->GetShape().left - m_Shape.left) };
	if (distanceToEnemie > m_MaxActionRadius)
	{
		// avatar
		if (distanceToAvatar > m_MinActionRadius and distanceToAvatar < m_MaxActionRadius and m_State != State::shooting and !m_IsDead)
		{
			if (m_Shape.left < avatarPosition.x)
			{
				m_Velocity.x = m_HorizontalSpeed;
			}
			if (m_Shape.left > avatarPosition.x)
			{
				m_Velocity.x = -m_HorizontalSpeed;
			}
			m_SecPerFrame = m_SecPerFrameRunning;
		}
		else {
			m_Velocity.x = 0;
		}

		if (distanceToAvatar > m_MinActionRadius and distanceToAvatar < m_WalkingRadius and m_State != State::shooting and !m_IsDead)
		{
			if (m_Shape.left < avatarPosition.x)
			{
				m_Velocity.x = 200;
			}
			if (m_Shape.left > avatarPosition.x)
			{
				m_Velocity.x = -200;
			}
			m_SecPerFrame = m_SecPerFrameWalking;
		}

	}
	else
	{
		// enemies
		if (distanceToEnemie > m_MinActionRadius and distanceToEnemie < m_MaxActionRadius and m_State != State::shooting and !m_IsDead)
		{
			if (m_Shape.left < enemies[m_ClosestEnemieIdx]->GetShape().left)
			{
				m_Velocity.x = m_HorizontalSpeed;
			}
			if (m_Shape.left > enemies[m_ClosestEnemieIdx]->GetShape().left)
			{
				m_Velocity.x = -m_HorizontalSpeed;
			}
			m_SecPerFrame = m_SecPerFrameRunning;
		}
		else {
			m_Velocity.x = 0;
		}
	}
}

void Marine::DrawTexture() const
{
	Rectf sourceRect{}, destenationRect{};
	float frameWidth{}, fraweHeigth{};
	if (m_IsDead)
	{
		frameWidth = m_pTextureDead->GetWidth() / m_DeadSpriteCollumns;
		fraweHeigth = m_pTextureDead->GetHeight() / m_DeadSpriteRows;
	}
	else
	{
		frameWidth = m_pTexture->GetWidth() / m_SpriteCollumns;
		fraweHeigth = m_pTexture->GetHeight() / m_SpriteRows;
	}

	sourceRect.left = frameWidth * m_CurrentFrame;
	sourceRect.bottom = fraweHeigth * int(m_State);
	sourceRect.width = frameWidth;
	sourceRect.height = fraweHeigth;

	destenationRect.left = 0;
	destenationRect.bottom = 0;
	destenationRect.width = frameWidth * m_Scale;
	destenationRect.height = fraweHeigth * m_Scale;

	if (m_State == State::shooting)
	{
		sourceRect.left = 0;
		sourceRect.bottom = fraweHeigth + fraweHeigth * m_CurrentFrame;
		sourceRect.width = frameWidth;
		sourceRect.height = fraweHeigth;
	}
	if (m_State == State::shooting and m_pWeapon->GetBulletsLeft() == 0)
	{
		sourceRect.left = 0;
		sourceRect.bottom = fraweHeigth;
		sourceRect.width = frameWidth;
		sourceRect.height = fraweHeigth;
	}
	if (m_State == State::dead)
	{
		sourceRect.left = frameWidth * m_CurrentFrame;
		sourceRect.bottom = fraweHeigth;
		sourceRect.width = frameWidth;
		sourceRect.height = fraweHeigth;

		if (m_HasDeadAnimLooped)
		{
			sourceRect.left = frameWidth * (m_NrDeadFrames - 1);
		}
	}

	// X and Y ofset
	float offsetX{ -60 };
	float offsetY{ -8 };
	if (m_IsDead)
	{
		offsetY *= 2;
	}

	glPushMatrix();
	glTranslatef(m_Shape.left + offsetX * m_Scale, m_Shape.bottom + offsetY * m_Scale, 0);
	if (m_IsDead)
	{
		m_pTextureDead->Draw(destenationRect, sourceRect);
	}
	else
	{
		m_pTexture->Draw(destenationRect, sourceRect);
	}
	glPopMatrix();
}

void Marine::UpdateTexture(float elapsedSec)
{
	m_AccuTime += elapsedSec;
	int nrFrames{ int(m_AccuTime / m_SecPerFrame) };

	const float secPerFrame = (60 / m_pWeapon->GetFireRate()) / 2;
	int nrFrame{ int(m_AccuTime / secPerFrame) };

	switch (m_State)
	{
	case State::waiting:
		m_CurrentFrame = nrFrames % m_NrWaitningFrames;
		break;

	case State::shooting:
		m_CurrentFrame = nrFrame % m_NrShootingFrames;
		break;

	case State::running:
		m_CurrentFrame = nrFrames % m_NrRunningFrames;
		break;

	case State::dead:
		m_CurrentFrame = nrFrames % m_NrDeadFrames;
		if (m_CurrentFrame == m_NrDeadFrames - 1) {
			m_HasDeadAnimLooped = true;
		}
		break;

	default:
		break;
	}
}

void Marine::UpdateState()
{
	float velocity = abs(m_Velocity.x);
	if (velocity == 0)
	{
		m_State = State::waiting;
	}
	if (velocity > 0)
	{
		m_State = State::running;
	}
	if(m_IsInShootingRange)
	{
		m_State = State::shooting;
	}

	if (m_IsDead)
	{
		m_State = State::dead;
	}
}

void Marine::HandleShooting()
{
	float WeaponLength{ 28 };
	float WeaponHeight{ 76 };
	Point2f m_BulletSpawnPos{
		m_Shape.left + m_Shape.width + WeaponLength * m_Scale,
		m_Shape.bottom + WeaponHeight * m_Scale
	};

	if (m_IsLookingLeft)
	{
		m_BulletSpawnPos.x = m_Shape.left - WeaponLength * m_Scale;
	}

	if (m_State == State::shooting)
	{
		m_pWeapon->Shoot(m_BulletSpawnPos, m_ShootingAngle, m_IsLookingLeft);
	}
}

void Marine::CheckClosestEnemy(const std::vector<BaseEnemy*>& enemies)
{
	int distanceToClosestEnemie{ MAXINT };
	for (int i{}; i < enemies.size(); ++i)
	{
		if (!enemies[i]->GetIsDead())
		{
			if (abs(enemies[i]->GetShape().left - m_Shape.left) < distanceToClosestEnemie)
			{
				distanceToClosestEnemie = abs(enemies[i]->GetShape().left - m_Shape.left);
				m_ClosestEnemieIdx = i;
			}
		}
	}
}

void Marine::CalcShootingAngle(const std::vector<BaseEnemy*>& enemies)
{
	float adjacent = abs((m_Shape.left + m_Shape.width / 2) - (enemies[m_ClosestEnemieIdx]->GetShape().left + m_Shape.width / 2));
	float hypotenuse = abs(float(sqrt(pow(adjacent, 2) + pow((m_Shape.bottom + m_Shape.height / 2) - (enemies[m_ClosestEnemieIdx]->GetShape().bottom + enemies[m_ClosestEnemieIdx]->GetShape().height / 2), 2))));

	if (m_IsLookingLeft) {
		m_ShootingAngle = acos(adjacent / hypotenuse) * float(180.0f / M_PI);
		if (m_ShootingAngle > m_MaxShootingAngle) {
			m_ShootingAngle = m_MaxShootingAngle;
		}
	}
	else {
		m_ShootingAngle = acos(adjacent / hypotenuse) * float(180.0f / M_PI);
		if (m_ShootingAngle > m_MaxShootingAngle) {
			m_ShootingAngle = m_MaxShootingAngle;
		}
	}

	if (enemies[m_ClosestEnemieIdx]->GetShape().bottom + enemies[m_ClosestEnemieIdx]->GetShape().height / 2 < m_Shape.bottom + m_Shape.height / 2)
	{
		m_ShootingAngle *= -1;
	}
}

void Marine::CHeckForDrops(const std::vector<BaseObject*>& objects)
{
	if (m_IsDead and !m_HasDropedItems)
	{
		// 33% chance
		int randomNumber{ (rand() % 3) + 1 };
		switch (randomNumber)
		{
		case 1:
			break;

		default:
			break;
		}
		m_HasDropedItems = true;
	}
}

void Marine::CheckIsInShootingRange(const std::vector<BaseEnemy*>& enemies)
{
	if (abs(enemies[m_ClosestEnemieIdx]->GetShape().left - m_Shape.left) <= m_ShootingRadius
		and enemies[m_ClosestEnemieIdx]->GetShape().bottom + enemies[m_ClosestEnemieIdx]->GetShape().height >= m_Shape.bottom
		and enemies[m_ClosestEnemieIdx]->GetShape().bottom <= m_Shape.bottom + m_Shape.height)
	{
		m_IsInShootingRange = true;
	}
	else
	{
		m_IsInShootingRange = false;
	}
}

void Marine::CheckFollow()
{

}
