#pragma once
#include "BaseGame.h"
#include "SoundStream.h"
#include "SoundEffect.h"
#include "Avatar.h"
#include "Level.h"
#include "Camera.h"
#include "HUD.h"
#include "BaseEnemy.h"
#include "BaseObject.h"
#include "FragGrenade.h"
#include "Marine.h"
#include "StartScreen.h"
#include "Menu.h"

class BulletCasing;

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec, SDL_Event& e ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override;

private:
	// music
	int			 m_MaxVolume;
	int			 m_MusicVolume;
	int			 m_SFXVolume;

	SoundStream* m_pStartScreenMusic;
	SoundStream* m_pLevelMusic;
	SoundStream* m_pGameOverMusic;
	SoundStream* m_pEndMusic;

	bool		 m_IsPlayingLevelMusic;
	bool		 m_IsPlayingGameOverMusic;
	bool		 m_IsPlayingEndMusic;

	// other
	const float  m_Scale;
	float		 m_ConsoleTime;
	Point2f		 m_mousePos;
	Point2f		 m_savedMousepos;
	bool		 m_CanReset;

	bool		m_QuitGame;

	// startScreen
	StartScreen* m_pStartScreen;

	//Menu
	Menu*		m_pMenu;

	// HUD
	HUD*		m_pHUD;

	// level
	Level*		m_pLevel;
	bool		m_HasReachedEnd;

	//camera
	Camera*		m_pCamera;

	// avatar
	Avatar*		m_pAvatar;

	// marines
	std::vector<Marine*> m_pMarines;

	// enemies
	std::vector<BaseEnemy*> m_pEnemies;

	// objects
	std::vector<BaseObject*> m_pItems;

	// screens
	Color4f		m_ScreenOverlayColor;
	const float	m_ScreenOverlayTime;
	bool		m_CanDrawOverlayTexture;
	// endScreen
	Texture*	m_pEndTexture;
	// game over screen
	Texture*	m_pGameOverTexture;
	// restart
	Texture*	m_pRestartTexture;
	const float	m_RestartTextTime;
	Color4f		m_RestartColor;

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	// info
	void Info() const;

	// marines
	void LoadMarines();

	// enemies
	void LoadEnemies();

	// objects
	void LoadObjects();

	// screens
	// overlayScreen
	void DrawOverlayScreen(const Texture& screenImage) const;
	void UpdateOverlayScreen(float elapsedSec);	
	// reset text
	void DrawResartText() const;
	void UpdateRestartText(float elapsedSec);

	// reset
	void Reset();

	// music
	void HandleMusic();
	void CheckChangeMusicVolume();
	void CheckChangeSFXVolume();
	void SetMusicVolume();
	void SetSFXVolume();

	// settings file
	void InitVolumeSettings();
	void SaveVolumeSettings();
};