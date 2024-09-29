#pragma once
#include "utils.h"
#include "Texture.h"

class Avatar;
class HUD
{
public:
	HUD(const Rectf& viewPort, float scale);
	~HUD();
	void Draw() const;
	void Update(const Avatar& avatar, float elapsedSec);

private:
	const Rectf m_ViewPort;
	const float m_Scale;

	// other
	int		m_FontSize;
	bool	m_DrawPlasmaWeaponUI;
	bool	m_DrawFragUI;
	bool	m_DrawAutoRifleUI;
	bool	m_DrawNeedlerUI;
	int		m_NrOfBulletsInMag;
	int		m_NrOfTotalBulletsPerMag;
	int		m_NrOfGrenades;
	int		m_NrOfBulletsLeft;
	float	m_ShieldPercent;
	bool	m_DrawLowHealthIcon;
	float		m_ShieldLowTime;
	const float m_TimePerFlicker;

	// textures
	// shield
	Texture* m_pShieldIcon;
	Texture* m_pShieldOutline;
	Texture* m_pShieldBackGroundFill;
	Texture* m_pShieldFill;

	Texture* m_pShieldIconLow;
	Texture* m_pShieldOutlineLow;

	// health
	Texture* m_pHealthIcon;
	Texture* m_pHealthTexture;

	Texture* m_pHealthIconLow;

	// bullets
	Texture* m_pUITopBack;
	Texture* m_pNrOfBulletsLeft;
	Texture* m_pPlasmaWeaponUI;
	Texture* m_UIPlasmaBar;
	Texture* m_pUIBattery;
	Texture* m_pUIBullet;

	Texture* m_UIAutoRifleEmpty;
	Texture* m_UIAutoRifleFull;
	Texture* m_UINeedlerEmpty;
	Texture* m_UINeedlerFull;

	// grenades
	Texture* m_pFragGrenadeIcon;
	Texture* m_pPlasmaGrenadeIcon;
	Texture* m_pNrOfGrenadesText;

	// destenation Rectangles
	// shield
	Rectf	m_ShieldIconDstRect;
	Rectf	m_ShieldOutlineDstRect;
	Rectf	m_ShieldBackGroundFillDstRect;
	Rectf	m_ShieldFillDstRect;
	Rectf	m_ShieldSourceRect;

	// health
	Rectf	m_HealthIconDstRect;
	Rectf	m_HealthTextureDstRect;
	int		m_MaxNrOfHealthTextures;
	int		m_NrOfHealthTextures;

	// bullets
	Rectf	m_UITopBackDstRect;
	Rectf	m_PlasmaWeaponUIDstRect;
	Rectf	m_UIPlasmaBarDstRect;
	Rectf	m_UIPlasmaBarSourceRect;
	Rectf	m_BulletTypeDstRect;

	Rectf	m_UIAutoRifleDstRect;
	Rectf	m_UINeedlerDstRect;

	// grenades
	Rectf	m_GrenadeIconDstRect;

	void UpdateShieldHUD(float shield);
	void UpdateHealthHUD(float health);

	void UpdateNrOfTotBullets(int nrOfTotBullets);

	void DrawGrenadeUI() const;
	void UpdateGrenadeText(int nrOfGrenades);

	void DrawBulletsInMagUI(int maxBullets, int bulletsInMag) const;
	void UpdatePlasmaBar(float max, float current);
};