#include "pch.h"
#include "BaseButton.h"
#include "Texture.h"

BaseButton::BaseButton(const std::string& buttonText, const Point2f& centerPos) :
	m_FontSize			{ 70 },
	m_TextColorActive	{ 0.5f, .7f, 1.f, 1 },
	m_TextColorInActive	{ 0.3f, .5f, 0.8f, 0.6f },
	m_pButtonText		{ buttonText },
	m_Texture			{ new Texture(buttonText, "Textures/Pixeboy-z8XGD.ttf", m_FontSize, m_TextColorInActive)},
	m_CenterPos			{ centerPos },
	m_DstRect			{	m_CenterPos.x - m_Texture->GetWidth() / 2,
							m_CenterPos.y - m_Texture->GetHeight() / 2,
							m_Texture->GetWidth(),
							m_Texture->GetHeight() },

	m_CanPlayHoverEffect{ true },
	m_pHoverEffect{ new SoundEffect("Sounds/hover.ogg") },
	m_pClickEffect{ new SoundEffect("Sounds/select.ogg") }
{
}

BaseButton::~BaseButton()
{
	delete m_Texture;
	m_Texture = nullptr;

	delete m_pHoverEffect;
	m_pHoverEffect = nullptr;

	delete m_pClickEffect;
	m_pClickEffect = nullptr;
}

void BaseButton::Draw() const
{
	m_Texture->Draw(m_DstRect);
}

void BaseButton::OnHover(const Point2f& mousePos)
{
	if (utils::IsPointInRect(mousePos, m_DstRect))
	{
		delete m_Texture;
		m_Texture = nullptr;
		m_Texture = new Texture(m_pButtonText, "Textures/Pixeboy-z8XGD.ttf", m_FontSize, m_TextColorActive);

		if (m_CanPlayHoverEffect)
		{
			m_pHoverEffect->Play(false);
			m_CanPlayHoverEffect = false;
		}
	}
	else
	{
		delete m_Texture;
		m_Texture = nullptr;
		m_Texture = new Texture(m_pButtonText, "Textures/Pixeboy-z8XGD.ttf", m_FontSize, m_TextColorInActive);

		m_CanPlayHoverEffect = true;
	}
}

void BaseButton::SetSFXVolume(int volume)
{
	m_pHoverEffect->SetVolume(volume);
	m_pClickEffect->SetVolume(volume);
}
