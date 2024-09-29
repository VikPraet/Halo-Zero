#pragma once
#include "VolumeSetting.h"
#include "Controls.h"

class Texture;
class Menu
{
public:
	Menu(const Rectf& viewPort);
	~Menu();
	
	void Draw() const;

	void CheckOnHover(const Point2f& mousePos);
	void CheckOnClick(const Point2f& mousPos);

	bool GetisActive() const;
	void SetActive(bool isAcvtive);

	void SetMenuVolume(int volume);

	void CheckVolumePercent();

	float GetMusicVolumePercent() const;
	float GetSFXVolumePercent() const;

	void SetMusicVolumePercent(int volumePercent);
	void SetSFXVolumePercent(int volumePercent);
private:
	Texture*		m_pBackGroundTexture;
	Rectf			m_DstRect;
	bool			m_IsActive;

	VolumeSetting*	m_pMusicVolumeSetting;
	VolumeSetting*	m_pSFXVolumeSetting;
	Controls*		m_pControlsSetting;

	Texture*		m_pControlsTexture;
	Rectf			m_ControlsDstRect;
};

