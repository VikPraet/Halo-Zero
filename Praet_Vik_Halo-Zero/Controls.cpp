#include "pch.h"
#include "Controls.h"
#include "Texture.h"
#include "utils.h"

Controls::Controls(const std::string& buttonText, const Point2f& centerPos) :
	BaseSetting(buttonText, centerPos),
	m_pControlsText			{ new Texture("view controls", "Textures/Pixeboy-z8XGD.ttf", m_FontSizeSmall, m_TextColorSmall) },
	m_ControlsTextDstRect	{ m_InteractRect.left + m_InteractRect.width - m_pControlsText->GetWidth(), 
							  m_InteractRect.bottom,
							  m_pControlsText ->GetWidth(),
							  m_pControlsText ->GetHeight()},
	m_ShowControls			{ false }
{
}

Controls::~Controls()
{
	delete m_pControlsText;
	m_pControlsText = nullptr;
}

void Controls::DrawOnHover() const
{
	m_pControlsText->Draw(m_ControlsTextDstRect);
}

void Controls::OnClick(const Point2f& mousePos)
{
	if (utils::IsPointInRect(mousePos, m_InteractRect) and !m_ShowControls)
	{
		m_ShowControls = true;
		m_pClickEffect->Play(false);
	}
	else if(m_ShowControls)
	{
		m_ShowControls = false;
		m_pClickEffect->Play(false);
	}
}

bool Controls::GetShowControls() const
{
	return m_ShowControls;
}

void Controls::SetShowControls(bool showControls)
{
	m_ShowControls = showControls;
}