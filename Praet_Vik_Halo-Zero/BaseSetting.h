#pragma once
#include "SoundEffect.h"

class Texture;
class BaseSetting
{
public:
	BaseSetting(const std::string& buttonText, const Point2f& centerPos);
	~BaseSetting();

	virtual void Draw() const;
	virtual void DrawOnHover() const = 0;

	virtual void OnHover(const Point2f& mousePos);

	virtual bool GetIsActive() const;

	virtual void SetSFXVolume(int volume);

protected:
	int				m_FontSize;
	int				m_FontSizeSmall;
	Color4f			m_TextColor;
	Color4f			m_TextColorSmall;
	Texture*		m_pTexture;
	Texture*		m_pInicatorTexture;

	float			m_HorizontalSpasing;
	bool			m_IsActive;
	const Point2f	m_CenterPos;
	const float		m_Width;
	const float		m_Height;

	Rectf			m_InteractRect;
	Rectf			m_IndicatorDstRect;
	Point2f			m_TextPos;

	// sounds
	bool			m_CanPlayHoverEffect;
	SoundEffect*	m_pHoverEffect;
	SoundEffect*	m_pClickEffect;
};