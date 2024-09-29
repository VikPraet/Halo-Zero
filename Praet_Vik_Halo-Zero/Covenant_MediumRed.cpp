#include "pch.h"
#include "Covenant_MediumRed.h"
#include "Avatar.h"
#include "PlasmaGrenadeObject.h"
#include "PlasmaRifle.h"
#include "Marine.h"

// static texture data
Texture* Covenant_MediumRed::m_pTexture{ nullptr };
Texture* Covenant_MediumRed::m_pTextureDead{ nullptr };
SoundEffect* Covenant_MediumRed::m_pDyingSound{ nullptr };
int Covenant_MediumRed::instanceCounter{ };

Covenant_MediumRed::Covenant_MediumRed(const Rectf& levelBoundaries, const Point2f& position, float scale) :
	// state data
	m_State					{ State::waiting },

	// weapon data
	m_pWeapon				{ new PlasmaRifle(levelBoundaries, false, scale) },
	m_ShootingAngle			{ },
	m_MaxShootingAngle		{ 45.f },
	m_FireRifleCounter		{ },

	// enemy data
	m_Scale					{ scale },
	m_Shape					{ position.x, position.y , 55.f, 115.f },
	m_LevelCollisionShape	{ },
	m_IsDead				{ false },
	m_HorizontalSpeed		{ 80 * m_Scale },
	m_Velocity				{ 0.f, 0.f },
	m_Acceleration			{ 0.0f * m_Scale, -1350.f * m_Scale },
	m_MinActionRadius		{ 260.f * m_Scale },
	m_MaxActionRadius		{ 480.f * m_Scale },
	m_ShootingRadius		{ 325.f * m_Scale },
	m_HasReachedClosesPoint	{ false },
	m_IsOnGround			{ },
	m_IsLookingLeft			{ true },
	m_Health				{ 100 },
	m_ClosestMarineIdx		{ },

	// sprite data
	m_SpriteCollumns		{ 6 },
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

	m_SecPerFrame			{ 0.17f },

	// itemDrops
	m_HasDropedItems{ false }
{
	if (m_pTexture == nullptr) {
		m_pTexture = new Texture("Textures/Covenant_MediumRed.png");
	}
	if (m_pTextureDead == nullptr) {
		m_pTextureDead = new Texture("Textures/Covenant_MediumRed_Dead.png");
	}
	if (m_pDyingSound == nullptr) {
		m_pDyingSound = new SoundEffect("Sounds/covMediumDeath.mp3");
	}
	++instanceCounter;

	m_Shape.left *= m_Scale;
	m_Shape.bottom *= m_Scale;
	m_Shape.width *= m_Scale;
	m_Shape.height *= m_Scale;

	m_LevelCollisionShape.width = 120 * m_Scale;
	m_LevelCollisionShape.height = m_Shape.height;
	m_LevelCollisionShape.bottom = m_Shape.bottom;
	m_LevelCollisionShape.left = m_Shape.left + m_Shape.width / 2 - m_LevelCollisionShape.width / 2;

	m_SavedPos.x = m_Shape.left;
	m_SavedPos.y = m_Shape.bottom;
}

Covenant_MediumRed::~Covenant_MediumRed()
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

	delete m_pDyingSound;
	m_pDyingSound = nullptr;

	m_pWeapon->CleanUp();

	delete m_pWeapon;
	m_pWeapon = nullptr;
}

void Covenant_MediumRed::Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, std::vector<BaseObject*>& objects, Level& level)
{
	float distanceToAvatar{ abs((avatar->GetShape().left + avatar->GetShape().width / 2) - (m_Shape.left + m_Shape.width / 2)) };
	int distanceToClosestEnemie{ MAXINT };
	for (int i{}; i < marines.size(); ++i)
	{
		if (!marines[i]->GetIsDead())
		{
			if (abs(marines[i]->GetShape().left - m_Shape.left) < distanceToClosestEnemie)
			{
				distanceToClosestEnemie = abs(marines[i]->GetShape().left - m_Shape.left);
				m_ClosestMarineIdx = i;
			}
		}
	}
	float distanceToMarine{ abs((marines[m_ClosestMarineIdx]->GetShape().left + marines[m_ClosestMarineIdx]->GetShape().width / 2) - (m_Shape.left + m_Shape.width / 2)) };


	// do collision detection
	level.HandleCollision(m_LevelCollisionShape, m_Velocity);
	m_IsOnGround = level.IsOnGround(m_LevelCollisionShape);

	if (distanceToMarine < distanceToAvatar and !marines[m_ClosestMarineIdx]->GetIsDead())
	{
		CheckForMove(marines[m_ClosestMarineIdx]->GetShape());
		CheckReachedEdge(marines[m_ClosestMarineIdx]->GetShape(), level);
	}
	else
	{
		CheckForMove(avatar->GetShape());
		CheckReachedEdge(avatar->GetShape(), level);
	}

	// check if enemy is dead
	CheckDead();

	// update shape position
	m_Shape.left = m_LevelCollisionShape.left + m_LevelCollisionShape.width / 2 - m_Shape.width / 2;
	m_Shape.bottom = m_LevelCollisionShape.bottom;

	// update position
	m_Velocity += m_Acceleration * elapsedSec;
	m_LevelCollisionShape.left += m_Velocity.x * elapsedSec;
	m_LevelCollisionShape.bottom += m_Velocity.y * elapsedSec;

	// calculate angle for bullets
	if (m_State == State::shooting)
	{
		if (distanceToMarine < distanceToAvatar and !marines[m_ClosestMarineIdx]->GetIsDead())
		{
			CalcShootingAngle(marines[m_ClosestMarineIdx]->GetShape());
		}
		else
		{
			CalcShootingAngle(avatar->GetShape());
		}
	}

	UpdateState();
	UpdateTexture(elapsedSec);

	if (!m_IsDead)
	{
		if (distanceToMarine < distanceToAvatar and !marines[m_ClosestMarineIdx]->GetIsDead())
		{
			if (marines[m_ClosestMarineIdx]->GetShape().left + marines[m_ClosestMarineIdx]->GetShape().width / 2 > m_Shape.left + m_Shape.width / 2)
			{
				m_IsLookingLeft = false;
			}
			else {
				m_IsLookingLeft = true;
			}
			CheckClosestPoint(marines[m_ClosestMarineIdx]->GetShape());
		}
		else
		{
			if (avatar->GetShape().left + avatar->GetShape().width / 2 > m_Shape.left + m_Shape.width / 2)
			{
				m_IsLookingLeft = false;
			}
			else {
				m_IsLookingLeft = true;
			}
			CheckClosestPoint(avatar->GetShape());
		}
		HandleShooting();
	}

	CHeckForDrops(objects);
	m_pWeapon->Update(elapsedSec, avatar, marines, level);
}

void Covenant_MediumRed::Draw() const
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

void Covenant_MediumRed::DrawWeapon() const
{
	m_pWeapon->Draw();
}

Rectf Covenant_MediumRed::GetShape() const
{
	return m_Shape;
}

bool Covenant_MediumRed::GetIsDead() const
{
	return m_IsDead;
}

void Covenant_MediumRed::CheckDead()
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

void Covenant_MediumRed::DecreaseHealth(float hitpoints)
{
	m_Health -= hitpoints;
}

BaseWeapon* Covenant_MediumRed::GetWeapon() const
{
	return m_pWeapon;
}

void Covenant_MediumRed::SetWeapon(BaseWeapon*& weapon)
{
	m_pWeapon = weapon;
}

void Covenant_MediumRed::CleanUp()
{
	Covenant_MediumRed::~Covenant_MediumRed();
}

void Covenant_MediumRed::SetSFXVolume(int volume)
{
	m_pDyingSound->SetVolume(volume);
}

void Covenant_MediumRed::CheckForMove(const Rectf& shape)
{
	Point2f shapeCenter{
		shape.left + shape.width / 2,
		shape.bottom + shape.height / 2,
	};

	float distanceBeweenShapesCenters{ abs(shapeCenter.x - (m_Shape.left + m_Shape.width / 2)) };

	if (distanceBeweenShapesCenters > m_MinActionRadius and distanceBeweenShapesCenters < m_MaxActionRadius and m_State != State::shooting and !m_IsDead)
	{
		if (m_Shape.left + m_Shape.width / 2 < shapeCenter.x)
		{
			m_Velocity.x = m_HorizontalSpeed;
		}
		if (m_Shape.left + m_Shape.width / 2 > shapeCenter.x)
		{
			m_Velocity.x = -m_HorizontalSpeed;
		}
	}
	else {
		m_Velocity.x = 0;
	}
}

void Covenant_MediumRed::CheckReachedEdge(const Rectf& shape, Level& level)
{
	Point2f shapeCenter{
		shape.left + shape.width / 2,
		shape.bottom + shape.height / 2,
	};
	const float extraRayLength{ 20 };


	if (m_IsOnGround)
	{
		m_SavedPos.x = m_Shape.left;
		m_SavedPos.y = m_Shape.bottom;
	}

	if (shapeCenter.x < m_Shape.left + m_Shape.width / 2)
	{
		if (!utils::Raycast(level.GetVertices()[0], Point2f{ m_Shape.left, m_Shape.bottom + m_Shape.height }, Point2f{ m_Shape.left, m_Shape.bottom - extraRayLength }, m_Hitinfo))
		{
			m_Shape.left = m_SavedPos.x;
			m_Shape.bottom = m_SavedPos.y;

			m_Velocity.x = 0;
			m_Velocity.y = 0;
		}
	}
	else if (shapeCenter.x > m_Shape.left + m_Shape.width / 2)
	{
		if (!utils::Raycast(level.GetVertices()[0], Point2f{ m_Shape.left + m_Shape.width, m_Shape.bottom + m_Shape.height }, Point2f{ m_Shape.left + m_Shape.width, m_Shape.bottom - extraRayLength }, m_Hitinfo))
		{
			m_Shape.left = m_SavedPos.x;
			m_Shape.bottom = m_SavedPos.y;

			m_Velocity.x = 0;
			m_Velocity.y = 0;
		}
	}
}

void Covenant_MediumRed::DrawTexture() const
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
	float offsetX{ -29 };
	float offsetY{ -4 };

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

void Covenant_MediumRed::UpdateTexture(float elapsedSec)
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

void Covenant_MediumRed::UpdateState()
{
	float velocity = abs(m_Velocity.x);
	if (velocity > 0)
	{
		m_State = State::running;
	}
	if (velocity == 0)
	{
		m_State = State::waiting;
	}
	if (m_HasReachedClosesPoint)
	{
		m_State = State::shooting;
	}

	if (m_IsDead)
	{
		m_State = State::dead;
	}
}

void Covenant_MediumRed::CheckClosestPoint(const Rectf& shape)
{
	float distanceBetweenShapes{ abs(shape.left - m_Shape.left) };
	float margin{ 10 * m_Scale };
	if (distanceBetweenShapes - margin <= m_MinActionRadius)
	{
		m_HasReachedClosesPoint = true;
	}
	if (m_HasReachedClosesPoint and distanceBetweenShapes >= m_ShootingRadius)
	{
		m_HasReachedClosesPoint = false;
	}
}

void Covenant_MediumRed::HandleShooting()
{
	float WeaponLength{ 23 };
	float WeaponHeigth{ 78 };
	Point2f m_BulletSpawnPos{
		m_Shape.left + m_Shape.width + WeaponLength * m_Scale,
		m_Shape.bottom + WeaponHeigth * m_Scale
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

void Covenant_MediumRed::CalcShootingAngle(const Rectf& shape)
{
	float adjacent = abs((m_Shape.left + m_Shape.width / 2) - (shape.left + m_Shape.width / 2));
	float hypotenuse = abs(float(sqrt(pow(adjacent, 2) + pow((m_Shape.bottom + m_Shape.height / 2) - (shape.bottom + shape.height / 2), 2))));

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

	if (shape.bottom + shape.height / 2 < m_Shape.bottom + m_Shape.height / 2)
	{
		m_ShootingAngle *= -1;
	}
}

void Covenant_MediumRed::CHeckForDrops(std::vector<BaseObject*>& objects)
{
	if (m_IsDead and !m_HasDropedItems)
	{
		// show droped weapon
		m_pWeapon->SetStaticPosition(Point2f{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom });
		m_pWeapon->SetIsOnGround(true);

		m_HasDropedItems = true;
	}
}
