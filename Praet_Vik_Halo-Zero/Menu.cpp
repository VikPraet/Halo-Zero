#include "pch.h"
#include "Menu.h"
#include "Texture.h"

Menu::Menu(const Rectf& viewPort):
	m_pBackGroundTexture	{ new Texture("Textures/MenuBackGround.png")},
	m_DstRect				{ viewPort },
	m_IsActive				{ false },

	m_pMusicVolumeSetting	{ new VolumeSetting("music volume", Point2f{viewPort.left + viewPort.width / 2, viewPort.bottom + viewPort.height / 2 + 125}) },
	m_pSFXVolumeSetting		{ new VolumeSetting("SFX volume", Point2f{viewPort.left + viewPort.width / 2, viewPort.bottom + viewPort.height / 2}) },
	m_pControlsSetting		{ new Controls("controls", Point2f{viewPort.left + viewPort.width / 2, viewPort.bottom + viewPort.height / 2 - 125}) },

	m_pControlsTexture		{ new Texture("Textures/controls.png")},
	m_ControlsDstRect		{ }
{
	m_ControlsDstRect.width = m_pControlsTexture->GetWidth();
	m_ControlsDstRect.height = m_pControlsTexture->GetHeight();
	m_ControlsDstRect.left = viewPort.width / 2 - m_ControlsDstRect.width / 2;
	m_ControlsDstRect.bottom = viewPort.height / 2 - m_ControlsDstRect.height / 2;
}

Menu::~Menu()
{
	delete m_pBackGroundTexture;
	m_pBackGroundTexture = nullptr;

	delete m_pMusicVolumeSetting;
	m_pMusicVolumeSetting = nullptr;

	delete m_pSFXVolumeSetting;
	m_pSFXVolumeSetting = nullptr;

	delete m_pControlsSetting;
	m_pControlsSetting = nullptr;

	delete m_pControlsTexture;
	m_pControlsTexture = nullptr;
}

void Menu::Draw() const
{
	m_pBackGroundTexture->Draw(m_DstRect);

	if (m_pControlsSetting->GetShowControls())
	{
		m_pControlsTexture->Draw(m_ControlsDstRect);
	}
	else
	{
		m_pMusicVolumeSetting->Draw();
		m_pSFXVolumeSetting->Draw();
		m_pControlsSetting->Draw();
		if (m_pMusicVolumeSetting->GetIsActive())
		{
			m_pMusicVolumeSetting->DrawOnHover();
		}
		if (m_pSFXVolumeSetting->GetIsActive())
		{
			m_pSFXVolumeSetting->DrawOnHover();
		}
		if (m_pControlsSetting->GetIsActive())
		{
			m_pControlsSetting->DrawOnHover();
		}
	}
}

void Menu::CheckOnHover(const Point2f& mousePos)
{
	m_pMusicVolumeSetting->OnHover(mousePos);
	m_pSFXVolumeSetting->OnHover(mousePos);
	m_pControlsSetting->OnHover(mousePos);
}

void Menu::CheckOnClick(const Point2f& mousePos)
{
	m_pMusicVolumeSetting->OnClick(mousePos);
	m_pSFXVolumeSetting->OnClick(mousePos);
	m_pControlsSetting->OnClick(mousePos);
}

bool Menu::GetisActive() const
{
	return m_IsActive;
}

void Menu::SetActive(bool isAcvtive)
{	
	m_IsActive = isAcvtive;
	m_pControlsSetting->SetShowControls(false);
}

void Menu::SetMenuVolume(int volume)
{
	m_pMusicVolumeSetting->SetSFXVolume(volume);
	m_pSFXVolumeSetting->SetSFXVolume(volume);
	m_pControlsSetting->SetSFXVolume(volume);
}

void Menu::CheckVolumePercent()
{
	m_pMusicVolumeSetting->CheckVolumePercent();
	m_pSFXVolumeSetting->CheckVolumePercent();
}


float Menu::GetMusicVolumePercent() const
{
	return m_pMusicVolumeSetting->GetMusicPercent();
}

float Menu::GetSFXVolumePercent() const
{
	return m_pSFXVolumeSetting->GetMusicPercent();
}

void Menu::SetMusicVolumePercent(int volumePercent)
{
	m_pMusicVolumeSetting->SetVolumePercent(volumePercent);
}

void Menu::SetSFXVolumePercent(int volumePercent)
{
	m_pSFXVolumeSetting->SetVolumePercent(volumePercent);
}
