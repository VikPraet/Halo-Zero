#include "pch.h"
#include "OptionsButton.h"
#include "Menu.h"

OptionsButton::OptionsButton(const std::string& buttonText, const Point2f& centerPos):
	BaseButton(buttonText, centerPos)
{
}

void OptionsButton::OnClick(const Point2f& mousePos, Menu& menu)
{
	if (utils::IsPointInRect(mousePos, m_DstRect))
	{
		menu.SetActive(true);
		m_pClickEffect->Play(false);
	}
	else
	{
		menu.SetActive(false);
	}
}
