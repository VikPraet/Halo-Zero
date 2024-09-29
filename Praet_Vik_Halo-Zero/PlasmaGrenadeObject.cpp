#include "pch.h"
#include "PlasmaGrenadeObject.h"
#include "utils.h"
#include "Texture.h"
#include "Avatar.h"

Texture* PlasmaGrenadeObject::m_pTexture{ nullptr };
SoundEffect* PlasmaGrenadeObject::m_pPickUpSound{ nullptr };
int PlasmaGrenadeObject::instanceCounter{ };

PlasmaGrenadeObject::PlasmaGrenadeObject(const Point2f& position, float scale):
	BaseObject(position, scale),
	m_IsPickedUp{ false }
{
	if (m_pTexture == nullptr) {
		m_pTexture = new Texture("Textures/PlasmaGrenade.png");
	}
	if (m_pPickUpSound == nullptr) {
		m_pPickUpSound = new SoundEffect("Sounds/grenadePickUp.mp3");
	}
	++instanceCounter;

	m_Shape.left = m_Position.x;
	m_Shape.bottom = m_Position.y;
	m_Shape.width = m_pTexture->GetWidth() * m_Scale;
	m_Shape.height = m_pTexture->GetHeight() * m_Scale;
}

PlasmaGrenadeObject::~PlasmaGrenadeObject()
{
	--instanceCounter;
	if (instanceCounter < 1) {
		delete m_pTexture;
		m_pTexture = nullptr;

		delete m_pPickUpSound;
		m_pPickUpSound = nullptr;
	}
}

void PlasmaGrenadeObject::Draw() const
{
	if (!m_IsPickedUp)
	{
		m_pTexture->Draw(m_Shape);
	}
}

void PlasmaGrenadeObject::Update(Avatar*& avatar)
{
	if (!m_IsPickedUp and
		utils::IsOverlapping(m_Shape, avatar->GetShape()) and
		avatar->GetNrOfPlasmaGrenades() < avatar->GetMaxNrOfPlasmaGrenades())
	{
		avatar->AddPlasmaGrenade();
		m_IsPickedUp = true;
		m_pPickUpSound->Play(0);
	}
}

void PlasmaGrenadeObject::CleanUp()
{
	PlasmaGrenadeObject::~PlasmaGrenadeObject();
}

void PlasmaGrenadeObject::SetSFXVolume(int volume)
{
	m_pPickUpSound->SetVolume(volume);
}