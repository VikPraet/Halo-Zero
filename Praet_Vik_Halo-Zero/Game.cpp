#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MedKit.h"
#include "Covenant_LittleYellow.h"
#include "Covenant_LittleRed.h"
#include "Covenant_MediumBlueShield.h"
#include "Covenant_MediumBlue.h"
#include "Covenant_MediumRed.h"
#include "Covenant_BigBlue.h"

#include <fstream>
#include <sstream>

#include "BulletCasing.h"

Game::Game(const Window& window)
	:BaseGame{ window },
	// music
	m_MaxVolume				{ },
	m_MusicVolume			{ },
	m_pStartScreenMusic		{ new SoundStream("Sounds/main_theme.ogg") },
	m_pLevelMusic			{ new SoundStream("Sounds/level1.ogg") },
	m_pGameOverMusic		{ new SoundStream("Sounds/gameover.ogg") },
	m_pEndMusic				{ new SoundStream("Sounds/credits.ogg") },

	m_IsPlayingLevelMusic	{ false },
	m_IsPlayingGameOverMusic{ false },
	m_IsPlayingEndMusic		{ false },

	// other
	m_Scale					{ 2.f },
	m_ConsoleTime			{ },
	m_mousePos				{ },
	m_savedMousepos			{ },
	m_CanReset				{ false },

	m_QuitGame				{ false },
	
	// startScreen
	m_pStartScreen			{ new StartScreen(GetViewPort())},

	 // Menu
	m_pMenu					{ new Menu(GetViewPort()) },

	//HUD
	m_pHUD					{ new HUD(GetViewPort(), m_Scale) },

	// camera
	m_pCamera				{ new Camera {GetViewPort().width, GetViewPort().height} },

	// level
	m_pLevel				{ new Level(m_Scale) },
	m_HasReachedEnd			{ false },

	// avatar
	m_pAvatar				{ new Avatar{ m_pLevel->GetLevelRectf(), m_Scale } },

	// marine
	m_pMarines				{ },

	// ememies
	m_pEnemies				{ },

	// objects
	m_pItems				{ },

	// screens
	m_ScreenOverlayColor	{ Color4f{0.f, 0.f, 0.f, 0.f} },
	m_ScreenOverlayTime		{ 6.5f }, // seconds
	m_CanDrawOverlayTexture	{ false },
	// end screen
	m_pEndTexture			{ new Texture("Textures/GAMEWON.png") },
	// game over screen
	m_pGameOverTexture		{ new Texture("Textures/GAMEOVER.png") },
	// resart
	m_pRestartTexture		{ new Texture("Textures/Restart.png") },
	m_RestartTextTime		{ 2.f }, // seconds
	m_RestartColor			{ Color4f{0.f, 0.f, 0.f, 1.f} }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	m_pCamera->SetLevelBoundaries(m_pLevel->GetLevelRectf());
	LoadMarines();
	LoadEnemies();
	LoadObjects();

	m_pStartScreenMusic->Play(true);

	m_MaxVolume = m_pStartScreenMusic->GetVolume();
	InitVolumeSettings();
}

void Game::Cleanup( )
{
	SaveVolumeSettings();

	delete m_pStartScreenMusic;
	m_pStartScreenMusic = nullptr;

	delete m_pLevelMusic;
	m_pLevelMusic = nullptr;

	delete m_pGameOverMusic;
	m_pGameOverMusic = nullptr;

	delete m_pEndMusic;
	m_pEndMusic = nullptr;

	delete m_pStartScreen;
	m_pStartScreen = nullptr;

	delete m_pMenu;
	m_pMenu = nullptr;

	delete m_pHUD;
	m_pHUD = nullptr;
	
	delete m_pLevel;
	m_pLevel = nullptr;

	delete m_pCamera;
	m_pCamera = nullptr;

	delete m_pAvatar;
	m_pAvatar = nullptr;

	for (int i{}; i < m_pMarines.size(); ++i)
	{
		delete m_pMarines[i];
		m_pMarines[i] = nullptr;
	}

	for (int i{}; i < m_pEnemies.size(); ++i)
	{
		m_pEnemies[i]->CleanUp();

		delete m_pEnemies[i];
		m_pEnemies[i] = nullptr;
	}

	for (int i{}; i < m_pItems.size(); ++i)
	{
		m_pItems[i]->CleanUp();

		delete m_pItems[i];
		m_pItems[i] = nullptr;
	}

	delete m_pEndTexture;
	m_pEndTexture = nullptr;

	delete m_pGameOverTexture;
	m_pGameOverTexture = nullptr;

	delete m_pRestartTexture;
	m_pRestartTexture = nullptr;
}

void Game::Update( float elapsedSec, SDL_Event& e)
{
	if (m_pAvatar->GetIsDead() or m_HasReachedEnd or m_pStartScreen->GetIsActive() or m_pMenu->GetisActive())
	{
		SDL_ShowCursor(SDL_ENABLE);
	}
	else
	{
		SDL_ShowCursor(SDL_DISABLE);
	}

	if(!m_pStartScreen->GetIsActive())
	{
		// music
		HandleMusic();
	}
	else
	{
		// check quit
		if (m_QuitGame)
		{
			e.type = SDL_QUIT;
		}
	}

	if (!m_pStartScreen->GetIsActive() and !m_pMenu->GetisActive())
	{
		// camera
		m_pCamera->Update(elapsedSec, m_pAvatar->GetShape());

		glPushMatrix();
		m_mousePos.x = m_savedMousepos.x + m_pCamera->Translate(m_pAvatar->GetShape()).x;
		m_mousePos.y = m_savedMousepos.y + m_pCamera->Translate(m_pAvatar->GetShape()).y;
		glPopMatrix();

		
		if (!m_HasReachedEnd and !m_CanDrawOverlayTexture)
		{
			// update enemies
			for (int i{}; i < m_pEnemies.size(); ++i)
			{
				m_pEnemies[i]->Update(elapsedSec, m_pAvatar, m_pMarines, m_pItems, *m_pLevel);
			}

			// update avatar
			m_pAvatar->Update(elapsedSec, m_pAvatar, m_mousePos, m_pEnemies, *m_pLevel);

			// upadte marine
			for (int i{}; i < m_pMarines.size(); ++i)
			{
				m_pMarines[i]->Update(elapsedSec, *m_pAvatar, m_pEnemies, m_pItems, *m_pLevel);
			}

			// update objects
			for (int i{}; i < m_pItems.size(); ++i)
			{
				m_pItems[i]->Update(m_pAvatar);
			}
		}

		// update HUD
		m_pHUD->Update(*m_pAvatar, elapsedSec);

		//check if end has been reached
		m_HasReachedEnd = m_pLevel->HasReachedEnd(m_pAvatar->GetShape());


		// check which screen to Update and when
		if (m_HasReachedEnd or m_pAvatar->GetIsDead())
		{
			UpdateOverlayScreen(elapsedSec);
		}
		if (m_CanReset)
		{
			UpdateRestartText(elapsedSec);
		}
	}
	CheckChangeMusicVolume();
	CheckChangeSFXVolume();
	m_pAvatar->SetVolume(m_SFXVolume);
	for (int i{}; i < m_pItems.size(); ++i)
	{
		m_pItems[i]->SetSFXVolume(m_SFXVolume);
	}
}

void Game::Draw( ) const
{
	ClearBackground( );
	if (m_pStartScreen->GetIsActive())
	{
		m_pStartScreen->Draw(m_pMenu->GetisActive());
	}
	else
	{
		// furthest Parallax backGround
		glPushMatrix();
		m_pCamera->TranslateParallaxTwo(m_pAvatar->GetShape());
		m_pLevel->DrawParallaxTwo();
		glPopMatrix();

		// closest Parallax backGround
		glPushMatrix();
		m_pCamera->TranslateParallaxOne(m_pAvatar->GetShape());
		m_pLevel->DrawParallaxOne();
		glPopMatrix();

		glPushMatrix();
		{
			m_pCamera->Translate(m_pAvatar->GetShape());
			// draw Level BackGround
			m_pLevel->DrawBackground();

			// draw enemies
			for (int i{}; i < m_pEnemies.size(); ++i)
			{
				m_pEnemies[i]->Draw();
			}

			// draw objects
			for (int i{}; i < m_pItems.size(); ++i)
			{
				m_pItems[i]->Draw();
			}

			// draw objects
			for (int i{}; i < m_pItems.size(); ++i)
			{
				m_pItems[i]->Draw();
			}

			// draw marine 
			for (int i{}; i < m_pMarines.size(); ++i)
			{
				m_pMarines[i]->Draw();
			}

			// draw avatar
			m_pAvatar->Draw(m_mousePos);

			// draw enemy bulltes / weapon
			for (int i{}; i < m_pEnemies.size(); ++i)
			{
				m_pEnemies[i]->DrawWeapon();
			}

			// draw bullets / weapon
			for (int i{}; i < m_pMarines.size(); ++i)
			{
				m_pMarines[i]->DrawWeapon();
			}

			// draw level foreGround
			m_pLevel->DrawForeground();

			if (!m_HasReachedEnd and !m_pMenu->GetisActive())
			{
				m_pAvatar->DrawCroshair(m_mousePos);
			}
		}
		glPopMatrix();

		// draw HUD
		m_pHUD->Draw();

		// check which screen to draw and when
		if (m_pAvatar->GetIsDead())
		{
			DrawOverlayScreen(*m_pGameOverTexture);
		}
		if (m_HasReachedEnd)
		{
			DrawOverlayScreen(*m_pEndTexture);
		}
		if (m_CanReset)
		{
			DrawResartText();
		}
	}


	if (m_pMenu->GetisActive())
	{
		m_pMenu->Draw();
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	switch ( e.keysym.sym )
	{
	case SDLK_i:
		Info();
	case SDLK_r:
		if (m_CanReset)
		{
			Reset();
		}
		break;

	case SDLK_f:
		m_pAvatar->CheckWeaponPickUp(m_pAvatar, m_pEnemies);
		break;

	case SDLK_ESCAPE:
		if (m_pMenu->GetisActive())
		{
			m_pMenu->SetActive(false);
		}
		else if(!m_pMenu->GetisActive())
		{
			m_pMenu->SetActive(true);
		}
		break;
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
	m_mousePos = Point2f{ float(e.x), float(e.y) };
	m_savedMousepos = m_mousePos;

	if (m_pStartScreen->GetIsActive() and !m_pMenu->GetisActive())
	{
		m_pStartScreen->CheckOnHover(Point2f{ float(e.x), float(e.y) });
	}
	if(m_pMenu->GetisActive())
	{
		m_pMenu->CheckOnHover(Point2f{ float(e.x), float(e.y) });
	}
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		if (m_pStartScreen->GetIsActive() and !m_pMenu->GetisActive())
		{
			m_pStartScreen->CHeckOnClick(Point2f{ float(e.x), float(e.y) }, *m_pMenu, m_QuitGame);
		}
		else if (m_pMenu->GetisActive())
		{
			m_pMenu->CheckOnClick(Point2f{ float(e.x), float(e.y) });
		}
		break;

	case SDL_BUTTON_RIGHT:
		if (!m_pStartScreen->GetIsActive() and !m_pMenu->GetisActive())
		{
			m_pAvatar->ThrowGrenade();
		}
		break;
	}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.f, 0.f, 0.f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::Info() const
{
	system("CLS");
	std::cout << "'A' left" << std::endl;
	std::cout << "'D' right" << std::endl;
	std::cout << "'S' crouch" << std::endl;
	std::cout << "'SPACE' jump" << std::endl;
	std::cout << "'F' pick up weapon" << std::endl;
	std::cout << "'LEFT MOUSE' shoot" << std::endl;
	std::cout << "'RIGHT MOUSE' throw grenade" << std::endl;
	std::cout << "'ESC' open menu" << std::endl;
}

void Game::LoadMarines()
{
	m_pMarines.push_back(new Marine{ m_pLevel->GetLevelRectf(), Point2f{435, 400}, m_Scale });
}

void Game::LoadEnemies()
{
	// loading enemies

	// little yellow covenant
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{1850, 70}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{2025, 70}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{2100, 70}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{2590, 90}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{3050, 100}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{3190, 100}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{3630, 70}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{4000, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{4340, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{4840, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{5200, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{6390, 380}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{7080, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{8200, 70}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{8380, 70}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{8850, 100}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleYellow{ m_pLevel->GetLevelRectf(), Point2f{9100, 70}, m_Scale });

	// little red covenant
	m_pEnemies.push_back(new Covenant_LittleRed{ m_pLevel->GetLevelRectf(), Point2f{3160, 100}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleRed{ m_pLevel->GetLevelRectf(), Point2f{4890, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleRed{ m_pLevel->GetLevelRectf(), Point2f{6420, 380}, m_Scale });
	m_pEnemies.push_back(new Covenant_LittleRed{ m_pLevel->GetLevelRectf(), Point2f{8800, 100}, m_Scale });

	// Medium Covenant Shield
	m_pEnemies.push_back(new Covenant_MediumBlueShield{ m_pLevel->GetLevelRectf(), Point2f{3400, 70}, m_Scale });
	m_pEnemies.push_back(new Covenant_MediumBlueShield{ m_pLevel->GetLevelRectf(), Point2f{5775, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_MediumBlueShield{ m_pLevel->GetLevelRectf(), Point2f{5950, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_MediumBlueShield{ m_pLevel->GetLevelRectf(), Point2f{7815, 70}, m_Scale });

	// Medium Blue covenant
	m_pEnemies.push_back(new Covenant_MediumBlue{ m_pLevel->GetLevelRectf(), Point2f{2600, 80}, m_Scale });
	m_pEnemies.push_back(new Covenant_MediumBlue{ m_pLevel->GetLevelRectf(), Point2f{5145, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_MediumBlue{ m_pLevel->GetLevelRectf(), Point2f{7145, 415}, m_Scale });
	m_pEnemies.push_back(new Covenant_MediumBlue{ m_pLevel->GetLevelRectf(), Point2f{7920, 70}, m_Scale });

	// Medium Red covenant
	m_pEnemies.push_back(new Covenant_MediumRed{ m_pLevel->GetLevelRectf(), Point2f{10200, 70}, m_Scale });

	// Big Blue covenant
	m_pEnemies.push_back(new Covenant_BigBlue{ m_pLevel->GetLevelRectf(), Point2f{9575, 70}, m_Scale });
}

void Game::LoadObjects()
{
	m_pItems.push_back(new MedKit(Point2f{ 5000, 405 }, m_Scale));
}

void Game::DrawOverlayScreen(const Texture& screenImage) const
{
	Rectf dstRect{
		0,
		0,
		screenImage.GetWidth() * m_Scale,
		screenImage.GetHeight() * m_Scale,
	};

	if (m_CanDrawOverlayTexture)
	{
		screenImage.Draw(dstRect);
	}
	utils::SetColor(m_ScreenOverlayColor);
	utils::FillRect(0, 0, GetViewPort().width, GetViewPort().height);
}

void Game::UpdateOverlayScreen(float elapsedSec)
{
	float colorIncrement{ (1 / m_ScreenOverlayTime * 2) * elapsedSec };

	if (m_ScreenOverlayColor.a >= 1)
	{
		m_ScreenOverlayColor.a = 1;
		m_CanDrawOverlayTexture = true;
	}
	if (m_CanDrawOverlayTexture)
	{
		m_ScreenOverlayColor.a -= colorIncrement;
		if (m_ScreenOverlayColor.a < 0)
		{
			m_ScreenOverlayColor.a = 0;
		}
	}
	else
	{
		m_ScreenOverlayColor.a += colorIncrement;
	}

	if (m_CanDrawOverlayTexture and m_ScreenOverlayColor.a <= 0)
	{
		m_CanReset = true;
	}
}

void Game::DrawResartText() const
{
	Rectf dstRect{
		(GetViewPort().width / 2) - (m_pRestartTexture->GetWidth() * m_Scale) / 2,
		100,
		m_pRestartTexture->GetWidth() * m_Scale,
		m_pRestartTexture->GetHeight() * m_Scale,
	};
	m_pRestartTexture->Draw(dstRect);

	utils::SetColor(m_RestartColor);
	utils::FillRect(dstRect);
}

void Game::UpdateRestartText(float elapsedSec)
{
	if (m_RestartColor.a > 0)
	{
		m_RestartColor.a -= (1 / m_RestartTextTime) * elapsedSec;
	}
}

void Game::Reset()
{
	// new avatar
	delete m_pAvatar;
	m_pAvatar = nullptr;
	m_pAvatar = new Avatar(m_pLevel->GetLevelRectf(), m_Scale);

	// reset / new camera
	delete m_pCamera;
	m_pCamera = nullptr;
	m_pCamera = new Camera(GetViewPort().width, GetViewPort().height);
	m_pCamera->SetLevelBoundaries(m_pLevel->GetLevelRectf());

	// new marines
	for (int i{}; i < m_pMarines.size(); ++i)
	{
		//m_pMarines[i]->CleanUp();

		delete m_pMarines[i];
		m_pMarines[i] = nullptr;
	}
	m_pMarines.clear();
	LoadMarines();

	// new enemies
	for (int i{}; i < m_pEnemies.size(); ++i)
	{
		m_pEnemies[i]->CleanUp();

		delete m_pEnemies[i];
		m_pEnemies[i] = nullptr;
	}
	m_pEnemies.clear();
	LoadEnemies();

	// new objects
	for (int i{}; i < m_pItems.size(); ++i)
	{
		m_pItems[i]->CleanUp();

		delete m_pItems[i];
		m_pItems[i] = nullptr;
	}
	m_pItems.clear();
	LoadObjects();

	// reset other variables
	m_CanReset = false;
	m_HasReachedEnd = false;
	m_CanDrawOverlayTexture = false;
	m_ScreenOverlayColor = Color4f{ 0.f, 0.f, 0.f, 0.f };
	m_RestartColor = Color4f{ 0.f, 0.f, 0.f, 1.f };

	// music
	m_IsPlayingGameOverMusic = false;
	m_IsPlayingEndMusic = false;
	m_pGameOverMusic->Stop();
	m_pEndMusic->Stop();
	m_pLevelMusic->Play(true);
}

void Game::HandleMusic()
{
	if (!m_IsPlayingLevelMusic)
	{
		m_pStartScreenMusic->Stop();
		m_pLevelMusic->Play(true);

		m_IsPlayingLevelMusic = true;
	}
	if (m_pAvatar->GetIsDead() and !m_IsPlayingGameOverMusic)
	{
		m_IsPlayingGameOverMusic = true;
		m_pLevelMusic->Stop();
		m_pGameOverMusic->Play(true);
	}
	if (m_HasReachedEnd and !m_IsPlayingEndMusic)
	{
		m_IsPlayingEndMusic = true;
		m_pLevelMusic->Stop();
		m_pEndMusic->Play(true);
	}
}

void Game::CheckChangeMusicVolume()
{
	if (m_MusicVolume != m_MaxVolume * m_pMenu->GetMusicVolumePercent())
	{
		SetMusicVolume();
	}
}

void Game::CheckChangeSFXVolume()
{
	if (m_SFXVolume != int(m_MaxVolume * m_pMenu->GetSFXVolumePercent()))
	{
		SetSFXVolume();
	}

	if (m_pAvatar->GetIsDead())
	{
		m_SFXVolume *= 0.25f;
		m_pAvatar->SetVolume(m_SFXVolume);
		m_pMenu->SetMenuVolume(m_SFXVolume);
		m_pStartScreen->SetVolume(m_SFXVolume);

		for (int i{}; i < m_pMarines.size(); ++i)
		{
			m_pMarines[i]->GetWeapon()->SetSFXVolume(m_SFXVolume);
		}

		for (int i{}; i < m_pEnemies.size(); ++i)
		{
			m_pEnemies[i]->GetWeapon()->SetSFXVolume(m_SFXVolume);
			m_pEnemies[i]->SetSFXVolume(m_SFXVolume);
		}

		for (int i{}; i < m_pItems.size(); ++i)
		{
			m_pItems[i]->SetSFXVolume(m_SFXVolume);
		}
	}
}

void Game::SetMusicVolume()
{
	m_MusicVolume = m_MaxVolume * m_pMenu->GetMusicVolumePercent();
	m_pStartScreenMusic->SetVolume(m_MusicVolume);
	m_pLevelMusic->SetVolume(m_MusicVolume);
	m_pGameOverMusic->SetVolume(m_MusicVolume);
}

void Game::SetSFXVolume()
{
	m_SFXVolume = m_MaxVolume * m_pMenu->GetSFXVolumePercent();
	m_pAvatar->SetVolume(m_SFXVolume);
	m_pMenu->SetMenuVolume(m_SFXVolume);
	m_pStartScreen->SetVolume(m_SFXVolume);

	for (int i{}; i < m_pMarines.size(); ++i)
	{
		m_pMarines[i]->GetWeapon()->SetSFXVolume(m_SFXVolume);
	}

	for (int i{}; i < m_pEnemies.size(); ++i)
	{
		m_pEnemies[i]->GetWeapon()->SetSFXVolume(m_SFXVolume);
		m_pEnemies[i]->SetSFXVolume(m_SFXVolume);
	}

	for (int i{}; i < m_pItems.size(); ++i)
	{
		m_pItems[i]->SetSFXVolume(m_SFXVolume);
	}
}

void Game::InitVolumeSettings()
{
	int musicVolume{}, sfxVolume{};

	std::ifstream inputFile("C:/00 DAE/2022-2023/Semester 2/Programming 2/01 Game/Praet_Vik_Halo-Zero/Resources/Settings.txt");
	if (!inputFile) {
		std::cerr << "Failed to open the file." << std::endl;
	}

	std::string line;
	while (std::getline(inputFile, line)) {
		if (line.find("music volume:") != std::string::npos) {
			musicVolume = std::stoi(line.substr(line.find(":") + 1));
		}
		else if (line.find("sfx volume:") != std::string::npos) {
			sfxVolume = std::stoi(line.substr(line.find(":") + 1));
		}
	}

	m_pMenu->SetMusicVolumePercent(musicVolume);
	m_pMenu->SetSFXVolumePercent(sfxVolume);

	inputFile.close();

	m_pMenu->CheckVolumePercent();
	SetMusicVolume();
	SetSFXVolume();
}

void Game::SaveVolumeSettings()
{
	std::ofstream outputFile("C:/00 DAE/2022-2023/Semester 2/Programming 2/01 Game/Praet_Vik_Halo-Zero/Resources/Settings.txt");
	if (!outputFile) {
		std::cerr << "Failed to open the file." << std::endl;
	}

	outputFile << "music volume: " << m_pMenu->GetMusicVolumePercent() * 100 << ";" << std::endl;
	outputFile << "sfx volume: " << m_pMenu->GetSFXVolumePercent() * 100 << ";" << std::endl;

	outputFile.close();
}

