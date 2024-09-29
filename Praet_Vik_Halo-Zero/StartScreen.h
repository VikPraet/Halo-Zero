#pragma once
#include "StartButton.h" 
#include "OptionsButton.h" 
#include "QuitButton.h"

class Texture;
class Menu;
class StartScreen
{
public:
	StartScreen(const Rectf& viewPort);
	~StartScreen();

	void Draw(bool isMenuActive) const;

	void CheckOnHover(const Point2f& mousePos);
	void CHeckOnClick(const Point2f& mousePos, Menu& menu, bool& quitGame);

	bool GetIsActive() const;

	void SetVolume(int volume);

private:
	Texture* m_pTexture;
	bool m_IsActive;

	const Point2f m_Position;
	const float m_Width;
	const float m_Height;

	StartButton*	m_pStartButton;
	OptionsButton*	m_pOptionsButton;
	QuitButton*		m_pQuitButton;
};