#include "pch.h"
#include "VolumeSetting.h"
#include "Texture.h"
#include "utils.h"

VolumeSetting::VolumeSetting(const std::string& buttonText, const Point2f& centerPos):
	BaseSetting(buttonText, centerPos),
	m_VolumePercent			{ 0 },
	m_VolumeIncrement		{ 10 },

	m_pVolumeArrowRight		{ new Texture("Textures/VolumeArrowRight.png")},
	m_pVolumeArrowLeft		{ new Texture("Textures/VolumeArrowLeft.png")},
	m_pVolumePercentText	{ new Texture(std::to_string(m_VolumePercent) + "%", "Textures/Pixeboy-z8XGD.ttf", m_FontSizeSmall, m_TextColorSmall)},

	m_DecreaseArrowDstrect	{ },
	m_IncreaseArrowDstrect	{ },
	m_VolumePercentDstRect	{ }
{
	float scale{ m_pVolumePercentText->GetHeight() / m_pVolumeArrowRight->GetHeight() };

	m_IncreaseArrowDstrect.width	= m_pVolumeArrowRight->GetWidth() * scale;
	m_IncreaseArrowDstrect.height	= m_pVolumePercentText->GetHeight();
	m_IncreaseArrowDstrect.left		= m_InteractRect.left + m_InteractRect.width - m_IncreaseArrowDstrect.width;
	m_IncreaseArrowDstrect.bottom	= m_InteractRect.bottom;

	m_DecreaseArrowDstrect.width	= m_pVolumeArrowLeft->GetWidth() * scale;
	m_DecreaseArrowDstrect.height	= m_pVolumePercentText->GetHeight();
	m_DecreaseArrowDstrect.left		= m_IncreaseArrowDstrect.left - 300;
	m_DecreaseArrowDstrect.bottom	= m_InteractRect.bottom;

	m_VolumePercentDstRect.width	= m_pVolumePercentText->GetWidth();
	m_VolumePercentDstRect.height	= m_pVolumePercentText->GetHeight();
	m_VolumePercentDstRect.left		= (m_DecreaseArrowDstrect.left + m_DecreaseArrowDstrect.width + m_IncreaseArrowDstrect.left) / 2 - m_pVolumePercentText->GetWidth() / 2;
	m_VolumePercentDstRect.bottom	= m_IncreaseArrowDstrect.bottom;


	m_IncreaseInteractRect.left		= (m_DecreaseArrowDstrect.left + m_DecreaseArrowDstrect.width + m_IncreaseArrowDstrect.left) / 2;
	m_IncreaseInteractRect.bottom	= m_IncreaseArrowDstrect.bottom;
	m_IncreaseInteractRect.width	= m_IncreaseArrowDstrect.left - (m_DecreaseArrowDstrect.left + m_DecreaseArrowDstrect.width + m_IncreaseArrowDstrect.left) / 2 + m_IncreaseArrowDstrect.width;
	m_IncreaseInteractRect.height	= m_IncreaseArrowDstrect.height;

	m_DecreaseInteractRect.left		= m_DecreaseArrowDstrect.left;
	m_DecreaseInteractRect.bottom	= m_DecreaseArrowDstrect.bottom;
	m_DecreaseInteractRect.width	= m_IncreaseArrowDstrect.left - (m_DecreaseArrowDstrect.left + m_DecreaseArrowDstrect.width + m_IncreaseArrowDstrect.left) / 2 + m_IncreaseArrowDstrect.width;
	m_DecreaseInteractRect.height	= m_DecreaseArrowDstrect.height;
}

VolumeSetting::~VolumeSetting()
{
	delete m_pVolumeArrowRight;
	m_pVolumeArrowRight = nullptr;

	delete m_pVolumeArrowLeft;
	m_pVolumeArrowLeft = nullptr;

	delete m_pVolumePercentText;
	m_pVolumePercentText = nullptr;
}

void VolumeSetting::DrawOnHover() const
{
	m_pVolumeArrowRight->Draw(m_IncreaseArrowDstrect);
	m_pVolumeArrowLeft->Draw(m_DecreaseArrowDstrect);
	m_pVolumePercentText->Draw(m_VolumePercentDstRect);
}

void VolumeSetting::OnClick(const Point2f& mousePos)
{
	bool changePercent{ false };
	if (utils::IsPointInRect(mousePos, m_DecreaseInteractRect))
	{
		m_VolumePercent -= m_VolumeIncrement;
		changePercent = true;
	
	}
	if (utils::IsPointInRect(mousePos, m_IncreaseInteractRect))
	{
		m_VolumePercent += m_VolumeIncrement;
		changePercent = true;
		
	}
	if (changePercent)
	{
		changePercent = false;
		CheckVolumePercent();
		m_pClickEffect->Play(false);
	}
}

float VolumeSetting::GetMusicPercent() const
{
	return (float(m_VolumePercent) / 100.f);
}

void VolumeSetting::SetVolumePercent(int volume)
{
	m_VolumePercent = volume;
}

void VolumeSetting::CheckVolumePercent()
{
	CheckMinMaxPercent();
	delete m_pVolumePercentText;
	m_pVolumePercentText = nullptr;
	m_pVolumePercentText = new Texture(std::to_string(m_VolumePercent) + "%", "Textures/Pixeboy-z8XGD.ttf", m_FontSizeSmall, m_TextColorSmall);
	m_VolumePercentDstRect.left = (m_DecreaseArrowDstrect.left + m_DecreaseArrowDstrect.width + m_IncreaseArrowDstrect.left) / 2 - m_pVolumePercentText->GetWidth() / 2;
	m_VolumePercentDstRect.width = m_pVolumePercentText->GetWidth();
}

void VolumeSetting::CheckMinMaxPercent()
{
	int minPercent{ 0 }, maxPercent{ 100 };
	if (m_VolumePercent > maxPercent)
	{
		m_VolumePercent = minPercent;
	}
	else if (m_VolumePercent < minPercent)
	{
		m_VolumePercent = maxPercent;
	}
}
