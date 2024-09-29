#include "pch.h"
#include "Avatar.h"
#include <iostream>

Avatar::Avatar(const Rectf& levelBoundaries, float scale) :
	// sound
	m_pDyingSound				{ new SoundEffect("Sounds/PlayerDeath.ogg") },
	m_pNewWeaponSound			{ new SoundEffect("Sounds/Pick-Up-Weapon.ogg") },
	m_pShieldLowSound			{ new SoundEffect("Sounds/ShieldLow.mp3") },
	m_pShieldRegenSound			{ new SoundEffect3("Sounds/ShieldRegen.mp3") },
	
	m_HasPlayedShieldLowSound	{ false },
	m_HasPlayedShieldRegenSound	{ false },
	m_HasStoppedShieldRegenSound{ false },

	// health data
	m_MaxHealth					{ 100 },
	m_Health					{ m_MaxHealth },
	m_MaxShield					{ 100 },
	m_Shield					{ m_MaxShield },
	m_ShieldRegenRate			{ 45.f },
	m_ShieldRegenCooldown		{ 3.0f },
	m_AccuShieldTime			{ },
	m_IsShieldOnCooldown		{ false },
	m_IsShieldRegen				{ false },
	m_IsDead					{ false },

	// Weapon
	m_pWeapon					{ new AutoRifle(levelBoundaries, true, scale) },
	m_LevelBoundaries			{ levelBoundaries },
	m_BulletSpawnPos			{ },
	groundToWeaponHeight		{ },
	m_FirstShot					{ true },

	// grenades
	m_NrOfFragGrenades			{ 3 },
	m_NrOfPlasmaGrenades		{ 0 },
	MAX_NR_OF_PLASMA_GRENADES	{ 3 },
	m_pGrenades					{ },

	// Avatar data
	m_Scale						{ scale },
	m_Shape						{ 350.f * m_Scale, 400.f * m_Scale, 35 * m_Scale, 105 * m_Scale },
	m_LevelCollisionShape{ },
	m_LegsState					{ LegsState::waiting },
	m_IsOnGround				{ },
	m_IsKeyPressed				{ },
	m_IsMousePressed			{ },
	m_Crouchdistance			{ 30 * m_Scale },
	m_AvatarStandingHeight		{ m_Shape.height },

	// looking angle
	m_LookAngle					{ 0.f },
	MAX_LOOK_ANGLE				{ 35.f },
	m_IsLookingBackwards				{ },

	// speed
	HORIZONTAL_RUNNING_SPEED	{ 195.f * m_Scale },
	HORIZONTAL_CROUCHING_SPEED	{ 100.f * m_Scale },
	JUMP_SPEED					{ 660.f * m_Scale },
	m_Velocity					{ },
	m_Acceleration				{ 0.0f * m_Scale, -1350.f * m_Scale },

	// sprite data
	m_pLegsSprite				{ new Texture("Textures/MasterChiefLegs.png") },
	m_pThrowTexture				{ new Texture("Textures/GrenadeTorso.png")},
	m_pDeadTexture				{ new Texture("Textures/MasterChiefDEAD.png") },
	SECONDS_PER_FRAME			{ 0.07f },
	m_TorsoDstRect				{ },
	m_LegsDstRect				{ },
	m_DeadDstRect				{ },

	// leg sprite
	LEG_SPRITE_FRAMES_WAITING	{ 1 },
	LEG_SPRITE_FRAMES_RUNNING	{ 9 },
	LEG_SPRITE_FRAMES_JUMPING	{ 1 },
	LEG_SPRITE_FRAMES_CROUCHING	{ 4 },
	LEG_SPRITE_COLUMNS			{ 9 },
	LEG_SPRITE_ROWS				{ 4 },
	m_CurrentLegFrame			{ },
	m_AccuTimeLegs				{ },

	// tosro sprite
	TORSO_SPRITE_SHOOTING_FRAMES{ 2 },
	TORSO_SPRITE_COLUMNS		{ 7 },
	TORSO_SPRITE_ROWS			{ 3 },
	m_CurrentTorsoFrame			{ },
	m_AccuTimeTorso				{ },


	// grenade torso
	TORSO_SPRITE_THROWING_FRAMES{ 5 },
	SECONDS_PER_THROWING_FRAME	{ 0.11f },
	m_CurrentThrowingFrame		{ },
	m_AccuTimeThrowing			{ },
	m_IsThrowingGrenade			{ false },
	m_CanThrowGrenadeCounter	{ },

	// daed sprite data
	DEAD_SPRITE_FRAMES			{ 3 },
	DEAD_SPRITE_COLUMNS			{ 3 },
	DEAD_SPRITE_ROWS			{ 1 },
	m_CurrentDeadFrame			{ },
	m_AccuTimeDead				{ },
	SEC_PER_DEAD_FRAME			{ 0.4f },
	m_HasDeadAnimLooped			{ false }
{
	m_TorsoDstRect.width = (m_pWeapon->GetTorsoSprite()->GetWidth() / TORSO_SPRITE_COLUMNS) * m_Scale;
	m_TorsoDstRect.height = (m_pWeapon->GetTorsoSprite()->GetHeight() / TORSO_SPRITE_ROWS) * m_Scale;

	m_LegsDstRect.width = (m_pLegsSprite->GetWidth() / LEG_SPRITE_COLUMNS) * m_Scale;
	m_LegsDstRect.height = (m_pLegsSprite->GetHeight() / LEG_SPRITE_ROWS) * m_Scale;

	m_DeadDstRect.width = (m_pDeadTexture->GetWidth() / DEAD_SPRITE_COLUMNS) * m_Scale;
	m_DeadDstRect.height = (m_pDeadTexture->GetHeight() / DEAD_SPRITE_ROWS) * m_Scale;

	m_LevelCollisionShape.width		= 108 * m_Scale;
	m_LevelCollisionShape.height	= 120 * m_Scale;
	m_LevelCollisionShape.bottom	= m_Shape.bottom;
	m_LevelCollisionShape.left		= m_Shape.left + m_Shape.width / 2 - m_LevelCollisionShape.width / 2;
}

Avatar::~Avatar()
{
	delete m_pDyingSound;
	m_pDyingSound = nullptr;

	delete m_pNewWeaponSound;
	m_pNewWeaponSound = nullptr;

	delete m_pShieldLowSound;
	m_pShieldLowSound = nullptr;

	delete m_pShieldRegenSound;
	m_pShieldRegenSound = nullptr;

	delete m_pLegsSprite;
	m_pLegsSprite = nullptr;

	delete m_pThrowTexture;
	m_pThrowTexture = nullptr;

	m_pWeapon->CleanUp();
	delete m_pWeapon;
	m_pWeapon = nullptr;

	delete m_pDeadTexture;
	m_pDeadTexture = nullptr;

	for (int i{}; i < m_pGrenades.size(); ++i)
	{
		m_pGrenades[i]->CleanUp();
		delete m_pGrenades[i];
		m_pGrenades[i] = nullptr;
	}
}

void Avatar::Update(float elapsedSec, Avatar*& avatar, const Point2f& mousePos, std::vector<BaseEnemy*>& enemies, Level& level)
{
	// check if avatar is dead
	CheckDead();

	// update shield
	handleShield(elapsedSec);

	// hitbox
	m_Shape.height = m_AvatarStandingHeight;
	if (m_LegsState == LegsState::staticCrouch || m_LegsState == LegsState::crouchWalking) {
		m_Shape.height -= m_Crouchdistance;
	}
	// set height of hitbox to make enemies shoot at ground
	const float deadHeight{ 20 };
	if (m_IsDead)
	{
		m_Shape.height = deadHeight;
	}

	// level colision
	level.HandleCollision(m_LevelCollisionShape, m_Velocity);
	m_IsOnGround = level.IsOnGround(m_LevelCollisionShape);

	m_Shape.left = m_LevelCollisionShape.left + m_LevelCollisionShape.width / 2 - m_Shape.width / 2;
	m_Shape.bottom = m_LevelCollisionShape.bottom;

	// movement
	m_Velocity += m_Acceleration * elapsedSec;
	m_LevelCollisionShape.bottom += m_Velocity.y * elapsedSec;


	if (!m_IsDead)
	{
		m_LevelCollisionShape.left += m_Velocity.x * elapsedSec;
		// check which direction to face
		if (mousePos.x < m_Shape.left + m_Shape.width / 2)
		{
			m_IsLookingBackwards = true;
		}
		else
		{
			m_IsLookingBackwards = false;
		}

		//handle movement
		HandleMovement();
		HandleStepHeight();

		// update sprites
		HandleLegs();
		UpdateLegsSprite(elapsedSec, mousePos);
		if (m_IsThrowingGrenade)
		{
			UpdateThrowingSprite(elapsedSec);
		}
		else
		{
			UpdateTorsoSprite(elapsedSec, mousePos);
		}

		// calculate rotation for torso
		CalcLookAngle(mousePos);

		// weapon
		CalcBulletSpawnPos(mousePos);
		HandleShooting(mousePos);
	}
	else
	{
		UpdateDeadSprite(elapsedSec);
	}

	for (int i{}; i < m_pGrenades.size(); ++i)
	{
		m_pGrenades[i]->Update(elapsedSec, avatar, enemies, level);
	}
	CheckThrowGrenade();
	m_pWeapon->Update(elapsedSec,enemies, level);

	// sounds
	HandleShieldSounds();
}

void Avatar::Draw(const Point2f& mousePos) const
{
	m_pWeapon->Draw();

	// draw avatar
	glPushMatrix();
	{
		if (m_IsLookingBackwards) {
			glTranslatef(2 * m_Shape.left + m_Shape.width, 0, 0);
			glScalef(-1, 1, 1);
		}
		if (m_LegsState == LegsState::staticCrouch || m_LegsState == LegsState::crouchWalking) {
			glTranslatef(0, -m_Crouchdistance, 0);
		}

		if (!m_IsDead)
		{
			if (m_IsThrowingGrenade)
			{
				DrawThrowingSprite();
			}
			else
			{
				DrawTorsoSprite();
			}
			DrawLegsSprite();
		}
		else
		{
			DrawDeadSprite();
		}
	}
	glPopMatrix();

	for (int i{}; i < m_pGrenades.size(); ++i)
	{
		m_pGrenades[i]->Draw();
	}

	// draw hitbox
	//utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	//utils::DrawRect(m_Shape);
	//utils::SetColor(Color4f{ 0.7f, 0.3f, 0.4f, 1.f });
	//utils::DrawRect(m_LevelColisionShape);
}

void Avatar::DrawCroshair(const Point2f& mousePos) const
{
	if (!m_IsDead)
	{
		// draw current weapon croshair
		m_pWeapon->DrawCroshair(mousePos);
	}
}

void Avatar::AddHealth(float healthPoints)
{
	m_Health += healthPoints;
	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}
}

Rectf Avatar::GetShape() const
{
	return m_Shape;
}

std::vector<BaseBullet*> Avatar::GetBullets() const
{
	return m_pWeapon->GetBullets();
}

float Avatar::GetMaxHealth() const
{
	return m_MaxHealth;
}

float Avatar::GetHealth() const
{
	return m_Health;
}

float Avatar::GetShield() const
{
	return m_Shield;
}

bool Avatar::GetIsDead() const
{
	return m_IsDead;
}

void Avatar::DecreaseHealth(float hitpoints)
{
	if (m_Shield > 0)
	{
		m_Shield -= hitpoints;
		m_IsShieldOnCooldown = true;
		m_AccuShieldTime = 0;
		if (m_Shield <= 0)
		{
			m_Shield = 0;
		}
	}
	else if (m_Shield <= 0)
	{
		m_Health -= hitpoints;
	}
}

void Avatar::HandleMovement()
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	m_LegsState = LegsState::waiting;
	m_Velocity.x = 0;
	m_IsKeyPressed = false;

	if (pStates[SDL_SCANCODE_SPACE] && m_IsOnGround)
	{
		m_Shape.bottom += 3;
		m_Velocity.y = JUMP_SPEED;
		m_IsOnGround = false;
		m_IsKeyPressed = true;
	}
	if (pStates[SDL_SCANCODE_A])
	{
		m_Velocity.x = -HORIZONTAL_RUNNING_SPEED;
		m_IsKeyPressed = true;
	}
	if (pStates[SDL_SCANCODE_D])
	{
		m_Velocity.x = HORIZONTAL_RUNNING_SPEED;
		m_IsKeyPressed = true;
	}
	if (pStates[SDL_SCANCODE_A] && pStates[SDL_SCANCODE_S] && m_IsOnGround)
	{
		m_Velocity.x = -HORIZONTAL_CROUCHING_SPEED;
		m_IsKeyPressed = true;
	}
	if (pStates[SDL_SCANCODE_D] && pStates[SDL_SCANCODE_S] && m_IsOnGround)
	{
		m_Velocity.x = HORIZONTAL_CROUCHING_SPEED;
		m_IsKeyPressed = true;
	}
}

void Avatar::HandleLegs()
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	if (!m_IsOnGround)
	{
		m_LegsState = LegsState::jumping;
	}
	if (!m_IsKeyPressed && m_IsOnGround)
	{
		m_LegsState = LegsState::waiting;
	}
	if (!pStates[SDL_SCANCODE_S] &&
		m_LegsState == LegsState::waiting &&
		m_IsOnGround &&
		m_IsKeyPressed)
	{
		m_LegsState = LegsState::running;
	}
	if (pStates[SDL_SCANCODE_S] && m_IsOnGround)
	{
		m_LegsState = LegsState::staticCrouch;
	}
	if (pStates[SDL_SCANCODE_S] && m_IsOnGround && m_IsKeyPressed)
	{
		m_LegsState = LegsState::crouchWalking;
	}
}

void Avatar::CalcLookAngle(const Point2f& mousePos)
{
	const float GROUND_TO_WEAPON_HEIGHT_STANDING{ 85 };
	const float GROUND_TO_WEAPON_HEIGHT_CROUCHING{ 55 };

	if (m_LegsState == LegsState::staticCrouch || m_LegsState == LegsState::crouchWalking) {
		groundToWeaponHeight = GROUND_TO_WEAPON_HEIGHT_CROUCHING * m_Scale;
	}
	else {
		groundToWeaponHeight = GROUND_TO_WEAPON_HEIGHT_STANDING * m_Scale;
	}

	float adjacent = mousePos.x - (m_Shape.left + m_Shape.width / 2);
	float hypotenuse = float(sqrt(pow(adjacent, 2) + pow(mousePos.y - (m_Shape.bottom + groundToWeaponHeight), 2)));

	if (mousePos.y >= m_Shape.bottom + groundToWeaponHeight &&
		mousePos.x >= m_Shape.left + m_Shape.width / 2) {
		m_LookAngle = acos(adjacent / hypotenuse) * float(180.0f / M_PI);
		if (m_LookAngle > MAX_LOOK_ANGLE) {
			m_LookAngle = MAX_LOOK_ANGLE;
		}
	}
	else if (mousePos.y < m_Shape.bottom + groundToWeaponHeight &&
		mousePos.x > m_Shape.left + m_Shape.width / 2) {
		m_LookAngle = -acos(adjacent / hypotenuse) * float(180.0f / M_PI);
		if (m_LookAngle < (MAX_LOOK_ANGLE * -1)) {
			m_LookAngle = (MAX_LOOK_ANGLE * -1);
		}
	}
	else if (mousePos.y >= m_Shape.bottom + groundToWeaponHeight &&
		mousePos.x <= m_Shape.left + m_Shape.width / 2) {
		m_LookAngle = acos(-adjacent / hypotenuse) * float(180.0f / M_PI);
		if (m_LookAngle > MAX_LOOK_ANGLE) {
			m_LookAngle = MAX_LOOK_ANGLE;
		}
	}
	else {
		m_LookAngle = -acos(-adjacent / hypotenuse) * float(180.0f / M_PI);
		if (-m_LookAngle > MAX_LOOK_ANGLE) {
			m_LookAngle = -MAX_LOOK_ANGLE;
		}
	}
}

void Avatar::CalcBulletSpawnPos(const Point2f& mousePos)
{
	float WeaponLength{ 45 };

	m_BulletSpawnPos.x = m_Shape.left + m_Shape.width / 2 + cos(m_LookAngle * float(M_PI / 180.0f)) * WeaponLength * m_Scale;
	m_BulletSpawnPos.y = m_Shape.bottom + groundToWeaponHeight + sin(m_LookAngle * float(M_PI / 180.0f)) * WeaponLength * m_Scale;

	if (mousePos.x < m_Shape.left + m_Shape.width / 2)
	{
		m_BulletSpawnPos.x = m_Shape.left + m_Shape.width / 2 - cos(m_LookAngle * float(M_PI / 180.0f)) * WeaponLength * m_Scale;
	}
}

void Avatar::DrawLegsSprite() const
{
	Rectf sourceRect{};
	const float frameWidth{ m_pLegsSprite->GetWidth() / LEG_SPRITE_COLUMNS };
	const float fraweHeigth{ m_pLegsSprite->GetHeight() / LEG_SPRITE_ROWS };

	sourceRect.left = frameWidth * m_CurrentLegFrame;
	sourceRect.bottom = fraweHeigth * int(m_LegsState);
	sourceRect.width = frameWidth;
	sourceRect.height = fraweHeigth;

	if (m_LegsState == LegsState::crouchWalking) {
		sourceRect.bottom = fraweHeigth * int(LegsState::staticCrouch);
	}

	// X and Y ofset
	float offsetX{ 25 };
	float offsetY{ 5 };

	glPushMatrix();
	glTranslatef(m_Shape.left - offsetX * m_Scale, m_Shape.bottom - offsetY * m_Scale, 0);
	m_pLegsSprite->Draw(m_LegsDstRect, sourceRect);
	glPopMatrix();
}

void Avatar::UpdateLegsSprite(float elapsedSec, const Point2f& mousePos)
{
	m_AccuTimeLegs += elapsedSec;
	int nrFrames{ int(m_AccuTimeLegs / SECONDS_PER_FRAME) };

	switch (m_LegsState)
	{
	case LegsState::waiting:
		m_CurrentLegFrame = nrFrames % LEG_SPRITE_FRAMES_WAITING;
		break;

	case LegsState::running:
		m_CurrentLegFrame = nrFrames % LEG_SPRITE_FRAMES_RUNNING;

		if (mousePos.x > m_Shape.left + m_Shape.width / 2 && m_Velocity.x < 0) {
			m_CurrentLegFrame = LEG_SPRITE_FRAMES_RUNNING - m_CurrentLegFrame;
		}
		if (mousePos.x < m_Shape.left + m_Shape.width / 2 && m_Velocity.x > 0) {
			m_CurrentLegFrame = LEG_SPRITE_FRAMES_RUNNING - m_CurrentLegFrame;
		}
		break;

	case LegsState::jumping:
		m_CurrentLegFrame = nrFrames % LEG_SPRITE_FRAMES_JUMPING;
		break;

	case LegsState::staticCrouch:
		if (m_CurrentLegFrame < 1) {
			m_CurrentLegFrame = nrFrames % LEG_SPRITE_FRAMES_CROUCHING;
		}
		else {
			m_CurrentLegFrame = 1;
		}
		break;

	case LegsState::crouchWalking:
		m_CurrentLegFrame = nrFrames % LEG_SPRITE_FRAMES_CROUCHING;
		if (m_CurrentLegFrame < 1) {
			m_CurrentLegFrame = 1;
		}
		if (mousePos.x > m_Shape.left + m_Shape.width / 2 && m_Velocity.x < 0) {
			m_CurrentLegFrame = LEG_SPRITE_FRAMES_CROUCHING - m_CurrentLegFrame;
		}
		if (mousePos.x < m_Shape.left + m_Shape.width / 2 && m_Velocity.x > 0) {
			m_CurrentLegFrame = LEG_SPRITE_FRAMES_CROUCHING - m_CurrentLegFrame;
		}
		break;

	default:
		break;
	}
}

void Avatar::DrawTorsoSprite() const
{
	Rectf sourceRect{};
	const float frameWidth{ m_pWeapon->GetTorsoSprite()->GetWidth() / TORSO_SPRITE_COLUMNS };
	const float fraweHeigth{ m_pLegsSprite->GetHeight() / TORSO_SPRITE_ROWS };

	sourceRect.left = 0;
	sourceRect.bottom = fraweHeigth + fraweHeigth * m_CurrentTorsoFrame;
	sourceRect.width = frameWidth;
	sourceRect.height = fraweHeigth;

	// X and Y ofset
	float offsetX{ 26 };
	float offsetY{ 43 };
	Point2f RotationCenter{ 39, 20 };

	glPushMatrix();
	glTranslatef(m_Shape.left - offsetX * m_Scale + RotationCenter.x * m_Scale, m_Shape.bottom + offsetY * m_Scale + RotationCenter.y * m_Scale, 0);
	glRotatef(m_LookAngle, 0, 0, 1);
	glTranslatef(-RotationCenter.x * m_Scale, -RotationCenter.y * m_Scale, 0);
	m_pWeapon->DrawTorso(m_TorsoDstRect, sourceRect);
	glPopMatrix();
}

void Avatar::UpdateTorsoSprite(float elapsedSec, const Point2f& mousePos)
{
	const Uint32 pStates = SDL_GetMouseState(nullptr, nullptr);

	// 60 / by farerate in RPM --> because 60 seconds per minute divided by to since there are 2 frames
	const float secPerFrame = (60 / m_pWeapon->GetFireRate()) / 2;

	m_AccuTimeTorso += elapsedSec;
	int nrFrames{ int(m_AccuTimeTorso / secPerFrame) };

	switch (pStates)
	{
	case SDL_BUTTON(1):
		if (m_FirstShot) {
			m_CurrentTorsoFrame = 1;
			m_FirstShot = false;
		}
		else {
			m_CurrentTorsoFrame = nrFrames % TORSO_SPRITE_SHOOTING_FRAMES;
		}
		m_IsMousePressed = true;
		break;
	default:
		m_CurrentTorsoFrame = 0;
		m_IsMousePressed = false;
		m_FirstShot = true;
		break;
	}

	if (m_pWeapon->GetBulletsLeft() == 0)
	{
		m_CurrentTorsoFrame = 0;
	}
}

void Avatar::DrawThrowingSprite() const
{
	Rectf dstRect{}, sourceRect{};
	const float frameWidth{ m_pThrowTexture->GetWidth() / TORSO_SPRITE_THROWING_FRAMES };
	const float fraweHeigth{ m_pThrowTexture->GetHeight()};

	sourceRect.left = frameWidth + frameWidth * m_CurrentThrowingFrame;
	sourceRect.bottom = fraweHeigth;
	sourceRect.width = frameWidth;
	sourceRect.height = fraweHeigth;

	dstRect.width = frameWidth * m_Scale;
	dstRect.height = fraweHeigth * m_Scale;

	// X and Y ofset
	float offsetX{ 52 };
	float offsetY{ 57 };

	glPushMatrix();
	glTranslatef(m_Shape.left - offsetX * m_Scale, m_Shape.bottom + offsetY * m_Scale, 0);
	m_pThrowTexture->Draw(dstRect, sourceRect);
	glPopMatrix();
}

void Avatar::UpdateThrowingSprite(float elapsedSec)
{
	m_AccuTimeThrowing += elapsedSec;
	int nrFrames{ int(m_AccuTimeThrowing / SECONDS_PER_THROWING_FRAME) };
	m_CurrentThrowingFrame = nrFrames % TORSO_SPRITE_THROWING_FRAMES;

	if (m_CurrentThrowingFrame == TORSO_SPRITE_THROWING_FRAMES - 1)
	{
		m_IsThrowingGrenade = false;
		m_AccuTimeThrowing = 0;
		m_CanThrowGrenadeCounter = 0;
	}
}

void Avatar::DrawDeadSprite() const
{
	Rectf sourceRect{};
	const float frameWidth{ m_pDeadTexture->GetWidth() / DEAD_SPRITE_COLUMNS };
	const float fraweHeigth{ m_pDeadTexture->GetHeight() / DEAD_SPRITE_ROWS };

	sourceRect.left = frameWidth * m_CurrentDeadFrame;
	sourceRect.bottom = fraweHeigth;
	sourceRect.width = frameWidth;
	sourceRect.height = fraweHeigth;

	glPushMatrix();
	glTranslatef(m_Shape.left, m_Shape.bottom, 0);
	m_pDeadTexture->Draw(m_DeadDstRect, sourceRect);
	glPopMatrix();
}

void Avatar::UpdateDeadSprite(float elapsedSec)
{
	if (m_CurrentDeadFrame == DEAD_SPRITE_FRAMES - 1) {
		m_HasDeadAnimLooped = true;
	}

	if (!m_HasDeadAnimLooped)
	{
		m_AccuTimeDead += elapsedSec;
		int nrFrames{ int(m_AccuTimeDead / SEC_PER_DEAD_FRAME) };
		m_CurrentDeadFrame = nrFrames % DEAD_SPRITE_FRAMES;
	}
}

void Avatar::HandleShooting(const Point2f& mousePos)
{
	if (!m_IsThrowingGrenade)
	{
		m_pWeapon->Shoot(m_BulletSpawnPos, m_LookAngle, m_IsLookingBackwards);
	}
}

void Avatar::CheckThrowGrenade()
{
	if (m_CurrentThrowingFrame == 2)
	{
		++m_CanThrowGrenadeCounter;
	}
	if (m_CanThrowGrenadeCounter == 1)
	{
		Point2f grenadeStartPos{
			m_Shape.left + m_Shape.width / 2,
			m_Shape.bottom + 85 * m_Scale
		};

		m_CanThrowGrenadeCounter = 2;
		if (m_NrOfFragGrenades > 0)
		{
			--m_NrOfFragGrenades;
			m_pGrenades.push_back(new FragGrenade(grenadeStartPos, m_IsLookingBackwards, m_Scale));
		}
		else
		{
			--m_NrOfPlasmaGrenades;
			m_pGrenades.push_back(new PlasmaGrenade(grenadeStartPos, m_IsLookingBackwards, m_Scale));
		}
		
	}
}

void Avatar::ThrowGrenade()
{
	if (m_NrOfFragGrenades > 0 or m_NrOfPlasmaGrenades > 0)
	{
		m_IsThrowingGrenade = true;
	}
}

void Avatar::AddFragGrenade()
{
	++m_NrOfFragGrenades;
}

void Avatar::AddPlasmaGrenade()
{
	++m_NrOfPlasmaGrenades;
}

int Avatar::GetNrOfFragGrenades() const
{
	return m_NrOfFragGrenades;
}

int Avatar::GetNrOfPlasmaGrenades() const
{
	return m_NrOfPlasmaGrenades;
}

int Avatar::GetMaxNrOfPlasmaGrenades() const
{
	return MAX_NR_OF_PLASMA_GRENADES;
}

BaseWeapon* Avatar::GetWeapon() const
{
	return m_pWeapon;
}

void Avatar::handleShield(float elapsedSec)
{
	if (m_IsShieldOnCooldown)
	{
		m_AccuShieldTime += elapsedSec;
	}

	if (m_AccuShieldTime >= m_ShieldRegenCooldown and m_Health > 0)
	{
		m_IsShieldRegen = true;
		m_IsShieldOnCooldown = false;
		m_Shield += m_ShieldRegenRate * elapsedSec;
	}

	if (m_Shield > m_MaxShield)
	{
		m_Shield = m_MaxShield;
		m_IsShieldRegen = false;
	}
}

void Avatar::CheckDead()
{
	if (m_Health <= 0)
	{
		m_LegsState = LegsState::waiting;
		if (!m_IsDead)
		{
			m_AccuTimeTorso = 0;
			m_AccuTimeLegs = 0;

			m_pDyingSound->Play(0);
		}
		m_Health = 0;
		m_IsDead = true;
	}
}

void Avatar::HandleStepHeight()
{
	float stepHeight1{ 1.2f };
	float stepHeight2{ 2.6f };
	float stepHeight3{ 4.5f };
	if (m_IsOnGround and m_LegsState == LegsState::waiting)
	{
		switch (m_CurrentLegFrame)
		{
		case 1:
			m_Shape.bottom += stepHeight2;
			break;
		case 2:
			m_Shape.bottom += stepHeight3;
			break;
		case 3:
			m_Shape.bottom += stepHeight2;
			break;
		case 4:
			m_Shape.bottom += stepHeight1;
			break;
		case 6:
			m_Shape.bottom += stepHeight2;
			break;
		case 7:
			m_Shape.bottom += stepHeight3;
			break;
		case 8:
			m_Shape.bottom += stepHeight2;
			break;
		case 9:
			m_Shape.bottom += stepHeight1;
			break;
		default:
			break;
		}
	}
}

void Avatar::HandleShieldSounds()
{
	if (m_Shield == 0 and !m_HasPlayedShieldLowSound)
	{
		m_pShieldLowSound->Play( 8 );
		m_HasPlayedShieldLowSound = true;
	}
	else if (m_Shield > 0)
	{
		m_HasPlayedShieldLowSound = false;
	}

	if (m_IsShieldRegen and !m_HasPlayedShieldRegenSound)
	{
		m_pShieldRegenSound->Play(0);
		m_HasPlayedShieldRegenSound = true;
	}
	else if (m_IsShieldOnCooldown and m_HasPlayedShieldRegenSound or 
			 m_Shield >= m_MaxShield and m_HasPlayedShieldRegenSound)
	{
		m_pShieldRegenSound->StopAll();
		m_HasPlayedShieldRegenSound = false;

	}
}

void Avatar::SetWeapon(BaseWeapon*& weapon)
{
	m_pWeapon = weapon;
}

int Avatar::GetBulletsLeft() const
{
	return m_pWeapon->GetBulletsLeft();
}

void Avatar::CheckWeaponPickUp(Avatar*& avatar, std::vector<BaseEnemy*>& enemies)
{
	int enemieIdx{ };
	bool canPickUp{ false };
	for (int i{}; i < enemies.size(); ++i)
	{
		if (enemies[i]->GetIsDead() and
			utils::IsOverlapping(enemies[i]->GetWeapon()->GetSaticShape(), avatar->GetShape()))
		{
			enemieIdx = i;
			canPickUp = true;
		}
	}

	if (canPickUp)
	{
		if (m_IsLookingBackwards)
		{
			m_pWeapon->SetStaticPosition(Point2f{m_Shape.left - (m_Shape.width / 2), m_Shape.bottom + (m_Shape.height / 2)});
		}
		else
		{
			m_pWeapon->SetStaticPosition(Point2f{m_Shape.left + (m_Shape.width / 2), m_Shape.bottom + (m_Shape.height / 2)});
		}

		enemies[enemieIdx]->GetWeapon()->SetIsHeldByPlayer(true);
		enemies[enemieIdx]->GetWeapon()->SetIsOnGround(false);
		avatar->GetWeapon()->SetIsHeldByPlayer(false);
		avatar->GetWeapon()->SetIsOnGround(true);

		BaseWeapon* pTempWeapon1{ enemies[enemieIdx]->GetWeapon() };
		BaseWeapon* pTempWeapon2{ avatar->GetWeapon() };

		avatar->SetWeapon(pTempWeapon1);

		enemies[enemieIdx]->SetWeapon(pTempWeapon2);

		m_pNewWeaponSound->Play(0);
	}
}

Vector2f Avatar::GetVelocity() const
{
	return m_Velocity;
}

void Avatar::SetVolume(int volume)
{
	m_pWeapon->SetSFXVolume(volume);
	m_pDyingSound->SetVolume(volume);
	m_pNewWeaponSound->SetVolume(volume);
	m_pShieldLowSound->SetVolume(volume);
	m_pShieldRegenSound->SetVolume(volume);

	for (int i{}; i < m_pGrenades.size(); ++i)
	{
		m_pGrenades[i]->SetSoundVolume(volume);
	}
}