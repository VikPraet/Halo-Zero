#include "pch.h"
#include "StartScreen.h"
#include "Texture.h"
#include "Menu.h"

StartScreen::StartScreen(const Rectf& viewPort):
	m_pTexture			{ new Texture("Textures/startScreen.png") },
	m_IsActive			{ true },
	m_Position			{ viewPort.left , viewPort.bottom },
	m_Width				{ viewPort.width },
	m_Height			{ viewPort.height },
	m_pStartButton		{ new StartButton("PLay", Point2f{viewPort.left + viewPort.width / 2,  viewPort.bottom + 480}) },
	m_pOptionsButton	{ new OptionsButton("Settings", Point2f{viewPort.left + viewPort.width / 2,  viewPort.bottom + 380}) },
	m_pQuitButton		{ new QuitButton("Quit", Point2f{viewPort.left + viewPort.width / 2,  viewPort.bottom + 280}) }
{
}

StartScreen::~StartScreen()
{
	delete m_pTexture;
	m_pTexture = nullptr;

	delete m_pStartButton;
	m_pStartButton = nullptr;

	delete m_pOptionsButton;
	m_pOptionsButton = nullptr;

	delete m_pQuitButton;
	m_pQuitButton = nullptr;
}

void StartScreen::Draw(bool isMenuActive) const
{
	Rectf dstRect{
		m_Position.x,
		m_Position.y,
		m_Width,
		m_Height
	};
	m_pTexture->Draw(dstRect);

	if(!isMenuActive)
	{
		m_pStartButton->Draw();
		m_pOptionsButton->Draw();
		m_pQuitButton->Draw();
	}
}

void StartScreen::CheckOnHover(const Point2f& mousePos)
{
	m_pStartButton->OnHover(mousePos);
	m_pOptionsButton->OnHover(mousePos);
	m_pQuitButton->OnHover(mousePos);
}

void StartScreen::CHeckOnClick(const Point2f& mousePos, Menu& menu, bool& quitGame)
{
	m_pStartButton->OnClick(mousePos, m_IsActive);
	m_pOptionsButton->OnClick(mousePos, menu);
	m_pQuitButton->OnClick(mousePos, quitGame);
}

bool StartScreen::GetIsActive() const
{
	return m_IsActive;
}

void StartScreen::SetVolume(int volume)
{
	m_pStartButton->SetSFXVolume(volume);
	m_pOptionsButton->SetSFXVolume(volume);
	m_pQuitButton->SetSFXVolume(volume);
}
