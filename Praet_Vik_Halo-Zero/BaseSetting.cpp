#include "pch.h"
#include "BaseSetting.h"
#include "Texture.h"
#include "utils.h"

BaseSetting::BaseSetting(const std::string& buttonText, const Point2f& centerPos):
	m_FontSize			{ 65 },
	m_FontSizeSmall		{ 50 },
	m_TextColor			{ 1, 1, 1, 1},
	m_TextColorSmall	{ 0.3f, .5f, 0.8f, 1.f },
	m_pTexture			{ new Texture(buttonText, "Textures/Pixeboy-z8XGD.ttf", m_FontSize, m_TextColor) },
	m_pInicatorTexture	{ new Texture("Textures/SelectedIndicator.png")},

	m_HorizontalSpasing	{ 12 },
	m_IsActive			{ false },
	m_CenterPos			{ centerPos },
	m_Width				{ 500.f },
	m_Height			{ 100.f },

	m_InteractRect		{ centerPos.x - m_Width / 2,
						  centerPos.y - m_Height / 2,
						  m_Width,
						  m_Height },
	m_IndicatorDstRect	{ },
	m_TextPos			{ },

	m_CanPlayHoverEffect{ true },
	m_pHoverEffect		{ new SoundEffect("Sounds/hover.ogg")},
	m_pClickEffect		{ new SoundEffect("Sounds/select.ogg") }
{
	m_IndicatorDstRect.height = m_pTexture->GetHeight() + m_HorizontalSpasing;
	m_IndicatorDstRect.width = (m_IndicatorDstRect.height / m_pInicatorTexture->GetHeight()) * m_pInicatorTexture->GetWidth();
	m_IndicatorDstRect.left = m_InteractRect.left;
	m_IndicatorDstRect.bottom = m_InteractRect.bottom + m_InteractRect.height - m_IndicatorDstRect.height;

	m_TextPos.x = m_IndicatorDstRect.left + m_IndicatorDstRect.width / 2 - m_pTexture->GetWidth() / 2;
	m_TextPos.y = m_IndicatorDstRect.bottom + m_IndicatorDstRect.height / 2 - m_pTexture->GetHeight() / 2; 
}

BaseSetting::~BaseSetting()
{
	delete m_pTexture;
	m_pTexture = nullptr;

	delete m_pInicatorTexture;
	m_pInicatorTexture = nullptr;

	delete m_pHoverEffect;
	m_pHoverEffect = nullptr;

	delete m_pClickEffect;
	m_pClickEffect = nullptr;
}

void BaseSetting::Draw() const
{
	m_pTexture->Draw(m_TextPos);

	if (m_IsActive)
	{
		m_pInicatorTexture->Draw(m_IndicatorDstRect);
	}
}

void BaseSetting::OnHover(const Point2f& mousePos)
{
	if (utils::IsPointInRect(mousePos, m_InteractRect))
	{
		m_IsActive = true;
		if (m_CanPlayHoverEffect)
		{
			m_pHoverEffect->Play(false);
			m_CanPlayHoverEffect = false;
		}
	}
	else
	{
		m_IsActive = false;
		m_CanPlayHoverEffect = true;
	}
}

bool BaseSetting::GetIsActive() const
{
	return m_IsActive;
}

void BaseSetting::SetSFXVolume(int volume)
{
	m_pHoverEffect->SetVolume(volume);
	m_pClickEffect->SetVolume(volume);
}
