#include "pch.h"
#include "MedKit.h"
#include "utils.h"
#include "Texture.h"
#include "Avatar.h"

Texture* MedKit::m_pTexture{ nullptr };
SoundEffect* MedKit::m_pPickUpSound{ nullptr };
int MedKit::instanceCounter{ };

MedKit::MedKit(const Point2f& position, float scale):
	BaseObject		(position, scale),
	m_HealthValue	{ 100 },
	m_IsPickedUp	{ false }
{
	if (m_pTexture == nullptr) {
		m_pTexture = new Texture("Textures/HealthKit.png");
	}
	if (m_pPickUpSound == nullptr) {
		m_pPickUpSound = new SoundEffect("Sounds/medkit.mp3");
	}
	++instanceCounter;

	m_Shape.left	= m_Position.x * m_Scale;
	m_Shape.bottom	= m_Position.y * m_Scale;
	m_Shape.width	= m_pTexture->GetWidth() * m_Scale;
	m_Shape.height	= m_pTexture->GetHeight() * m_Scale;
}

MedKit::~MedKit()
{
	--instanceCounter;
	if (instanceCounter < 1) {
		delete m_pTexture;
		m_pTexture = nullptr;

		delete m_pPickUpSound;
		m_pPickUpSound = nullptr;
	}
}

void MedKit::Draw() const
{
	if (!m_IsPickedUp)
	{
		m_pTexture->Draw(m_Shape);
	}
}

void MedKit::Update(Avatar*& avatar)
{
	if (!avatar->GetIsDead() and 
		!m_IsPickedUp and utils::IsOverlapping(m_Shape, avatar->GetShape()) and
		avatar->GetHealth() < avatar->GetMaxHealth())
	{
		avatar->AddHealth(m_HealthValue);
		m_IsPickedUp = true;
		m_pPickUpSound->Play(0);
	}
}

void MedKit::CleanUp()
{
	MedKit::~MedKit();
}

void MedKit::SetSFXVolume(int volume)
{
	m_pPickUpSound->SetVolume(volume);
}
