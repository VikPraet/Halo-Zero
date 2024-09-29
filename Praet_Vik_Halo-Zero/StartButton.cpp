#include "pch.h"
#include "StartButton.h"

StartButton::StartButton(const std::string& buttonText, const Point2f& centerPos):
	BaseButton(buttonText, centerPos)
{
}

void StartButton::OnClick(const Point2f& mousePos, bool& StartGame)
{
	if (utils::IsPointInRect(mousePos, m_DstRect))
	{
		StartGame = false;
		m_pClickEffect->Play(false);
	}
	else
	{
		StartGame = true;
	}
}