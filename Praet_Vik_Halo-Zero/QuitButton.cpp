#include "pch.h"
#include "QuitButton.h"

QuitButton::QuitButton(const std::string& buttonText, const Point2f& centerPos):
	BaseButton(buttonText, centerPos)
{
}

void QuitButton::OnClick(const Point2f& mousePos, bool& quitGame)
{
	if (utils::IsPointInRect(mousePos, m_DstRect))
	{
		quitGame = true;
		m_pClickEffect->Play(false);
	}
}
