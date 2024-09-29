#include "pch.h"
#include "HUD.h"
#include "Avatar.h"
#include <iostream>
#include "AutoRifle.h"
#include "Needler.h"
#include "PlasmaPistol.h"
#include "PlasmaRifle.h"

HUD::HUD(const Rectf& viewPort, float scale) :
	m_ViewPort				{ viewPort },
	m_Scale					{ scale },

	// other
	m_FontSize				{ 22 },
	m_DrawPlasmaWeaponUI	{ false },
	m_DrawFragUI			{ true },
	m_DrawAutoRifleUI		{ true },
	m_DrawNeedlerUI			{ false },
	m_NrOfBulletsInMag		{ },
	m_NrOfTotalBulletsPerMag{ },
	m_NrOfGrenades			{ },
	m_NrOfBulletsLeft		{ },
	m_ShieldPercent			{ },
	m_DrawLowHealthIcon		{ false },
	m_ShieldLowTime			{ },
	m_TimePerFlicker		{ 0.16f },

	// textures
	// shield
	m_pShieldIcon			{ new Texture("Textures/UIPlayerShieldUp.png") },
	m_pShieldOutline		{ new Texture("Textures/UIPlayerShieldAround.png") },
	m_pShieldBackGroundFill	{ new Texture("Textures/UIPlayerShieldContour.png") },
	m_pShieldFill			{ new Texture("Textures/UIPlayerShields.png") },
	
	m_pShieldIconLow		{ new Texture("Textures/UIPlayerShieldUpLow.png") },
	m_pShieldOutlineLow		{ new Texture("Textures/UIPlayerShieldAroundLow.png") },

	// health
	m_pHealthIcon			{ new Texture("Textures/UIPlayerHealthIcon.png") },
	m_pHealthTexture		{ new Texture("Textures/UIPlayerHealth.png") },

	m_pHealthIconLow		{ new Texture("Textures/UIPlayerHealthIconLow.png") },

	// bullets
	m_pUITopBack			{ new Texture("Textures/UITopBack.png") },
	m_pNrOfBulletsLeft		{ new Texture("0", "Textures/Pixeboy-z8XGD.ttf", 60, Color4f{0.5f, .7f, 1.f, 1}) },
	m_pPlasmaWeaponUI		{ new Texture("Textures/UIPlasmaBulletLoader.png") },
	m_UIPlasmaBar			{ new Texture("Textures/UIPlasmaBar.png") },
	m_pUIBattery			{ new Texture("Textures/UIPlasmaBulletDisplay.png") },
	m_pUIBullet				{ new Texture("Textures/UIOtherBulletDisplay.png") },

	m_UIAutoRifleEmpty		{ new Texture("Textures/UIAutoRifleEmpty.png") },
	m_UIAutoRifleFull		{ new Texture("Textures/UIAutoRifleFull.png") },
	m_UINeedlerEmpty		{ new Texture("Textures/UINeedlerEmpty.png") },
	m_UINeedlerFull			{ new Texture("Textures/UINeedlerFull.png") },

	// grenades
	m_pFragGrenadeIcon		{ new Texture("Textures/UIGrenadeFragDisplay.png") },
	m_pPlasmaGrenadeIcon	{ new Texture("Textures/UIGrenadePlasmaDisplay.png") },
	m_pNrOfGrenadesText		{ new Texture("Textures/0", "Textures/Pixeboy-z8XGD.ttf", 60, Color4f{0.5f, .7f, 1.f, 1}) },

	// destenation Rectangles
	// shield
	m_ShieldIconDstRect				{ },
	m_ShieldOutlineDstRect			{ },
	m_ShieldBackGroundFillDstRect	{ },
	m_ShieldFillDstRect				{ },
	m_ShieldSourceRect				{ },

	// health
	m_HealthIconDstRect		{ },
	m_HealthTextureDstRect	{ },
	m_MaxNrOfHealthTextures	{ 8 },
	m_NrOfHealthTextures	{ m_MaxNrOfHealthTextures },

	// bullets
	m_UITopBackDstRect		{ },
	m_PlasmaWeaponUIDstRect	{ },
	m_UIPlasmaBarDstRect	{ },
	m_UIPlasmaBarSourceRect	{ },
	m_BulletTypeDstRect		{ },

	m_UIAutoRifleDstRect	{ },
	m_UINeedlerDstRect		{ },

	// grenades
	m_GrenadeIconDstRect	{ }
{
	// destenation Rectangles
	//shield
	m_ShieldOutlineDstRect.left		= viewPort.left + viewPort.width - 240;
	m_ShieldOutlineDstRect.bottom	= viewPort.bottom + viewPort.height - 90;
	m_ShieldOutlineDstRect.width	= m_pShieldOutline->GetWidth() * scale;
	m_ShieldOutlineDstRect.height	= m_pShieldOutline->GetHeight() * scale;

	m_ShieldBackGroundFillDstRect	= m_ShieldOutlineDstRect;
	m_ShieldFillDstRect				= m_ShieldOutlineDstRect;

	m_ShieldIconDstRect.width		= m_pShieldIcon->GetWidth()* scale;
	m_ShieldIconDstRect.height		= m_pShieldIcon->GetHeight()* scale;
	m_ShieldIconDstRect.left		= m_ShieldOutlineDstRect.left - m_ShieldIconDstRect.width - 3;
	m_ShieldIconDstRect.bottom		= m_ShieldOutlineDstRect.bottom + 27;


	m_ShieldSourceRect.width		= m_pShieldFill->GetWidth();
	m_ShieldSourceRect.height		= m_pShieldFill->GetHeight();

	// health
	m_HealthIconDstRect.width		= m_pHealthIcon->GetWidth() * scale;
	m_HealthIconDstRect.height		= m_pHealthIcon->GetHeight() * scale;
	m_HealthIconDstRect.left		= m_ShieldIconDstRect.left + 14;
	m_HealthIconDstRect.bottom		= m_ShieldIconDstRect.bottom - m_HealthIconDstRect.height - 10;

	m_HealthTextureDstRect.left		= m_ShieldOutlineDstRect.left + m_ShieldOutlineDstRect.width - 75;
	m_HealthTextureDstRect.bottom	= m_ShieldOutlineDstRect.bottom + 2;
	m_HealthTextureDstRect.width	= m_pHealthTexture->GetWidth() * scale;
	m_HealthTextureDstRect.height	= m_pHealthTexture->GetHeight() * scale;

	// bullets
	m_UITopBackDstRect.left		= viewPort.left + 50;
	m_UITopBackDstRect.bottom	= viewPort.bottom + viewPort.height - 63;
	m_UITopBackDstRect.width	= m_pUITopBack->GetWidth()* scale;
	m_UITopBackDstRect.height	= m_pUITopBack->GetHeight()* scale;

	m_PlasmaWeaponUIDstRect.width	= m_pPlasmaWeaponUI->GetWidth() * scale;
	m_PlasmaWeaponUIDstRect.height	= m_pPlasmaWeaponUI->GetHeight() * scale;
	m_PlasmaWeaponUIDstRect.left	= m_UITopBackDstRect.left - 15;
	m_PlasmaWeaponUIDstRect.bottom	= m_UITopBackDstRect.bottom - m_PlasmaWeaponUIDstRect.height - 5;

	m_UIPlasmaBarDstRect = m_PlasmaWeaponUIDstRect;

	m_UIPlasmaBarSourceRect.width = m_UIPlasmaBar->GetWidth();
	m_UIPlasmaBarSourceRect.height = m_UIPlasmaBar->GetHeight();

	m_BulletTypeDstRect.left	= m_UITopBackDstRect.left + 75;
	m_BulletTypeDstRect.bottom	= m_UITopBackDstRect.bottom + 5;
	m_BulletTypeDstRect.width	= m_pUIBullet->GetWidth()  * scale;
	m_BulletTypeDstRect.height	= m_pUIBullet->GetHeight() * scale;

	m_UIAutoRifleDstRect.left	= m_UITopBackDstRect.left - 5;
	m_UIAutoRifleDstRect.bottom = m_UITopBackDstRect.bottom - 20;
	m_UIAutoRifleDstRect.width	= m_UIAutoRifleEmpty->GetWidth() * scale;
	m_UIAutoRifleDstRect.height = m_UIAutoRifleEmpty->GetHeight() * scale;

	m_UINeedlerDstRect.left		= m_UITopBackDstRect.left - 10;
	m_UINeedlerDstRect.bottom	= m_UITopBackDstRect.bottom - 30;
	m_UINeedlerDstRect.width	= m_UINeedlerEmpty->GetWidth() * (scale * .9f);
	m_UINeedlerDstRect.height	= m_UINeedlerEmpty->GetHeight() * (scale * .9f);

	// grenades
	m_GrenadeIconDstRect.left	= m_UITopBackDstRect.left + m_UITopBackDstRect.width - 32;
	m_GrenadeIconDstRect.bottom = m_UITopBackDstRect.bottom + 6;
	m_GrenadeIconDstRect.width	= m_pFragGrenadeIcon->GetWidth() * scale;
	m_GrenadeIconDstRect.height = m_pFragGrenadeIcon->GetHeight() * scale;
}

HUD::~HUD()
{
	// shield
	delete m_pShieldIcon;
	m_pShieldIcon = nullptr;

	delete m_pShieldOutline;
	m_pShieldOutline = nullptr;

	delete m_pShieldBackGroundFill;
	m_pShieldBackGroundFill = nullptr;

	delete m_pShieldFill;
	m_pShieldFill = nullptr;

	delete m_pShieldIconLow;
	m_pShieldIconLow = nullptr;

	delete m_pShieldOutlineLow;
	m_pShieldOutlineLow = nullptr;

	// health
	delete m_pHealthIcon;
	m_pHealthIcon = nullptr;

	delete m_pHealthTexture;
	m_pHealthTexture = nullptr;

	delete m_pHealthIconLow;
	m_pHealthIconLow = nullptr;

	// bullets
	delete m_pUITopBack;
	m_pUITopBack = nullptr;
	
	delete m_pPlasmaWeaponUI;
	m_pPlasmaWeaponUI = nullptr;

	delete m_UIPlasmaBar;
	m_UIPlasmaBar = nullptr;

	delete m_pNrOfBulletsLeft;
	m_pNrOfBulletsLeft = nullptr;	
	
	delete m_pUIBattery;
	m_pUIBattery = nullptr;
	
	delete m_pUIBullet;
	m_pUIBullet = nullptr;


	delete m_UIAutoRifleEmpty;
	m_UIAutoRifleEmpty = nullptr;

	delete m_UIAutoRifleFull;
	m_UIAutoRifleFull = nullptr;

	delete m_UINeedlerEmpty;
	m_UINeedlerEmpty = nullptr;

	delete m_UINeedlerFull;
	m_UINeedlerFull = nullptr;

	// grenades
	delete m_pFragGrenadeIcon;
	m_pFragGrenadeIcon = nullptr;

	delete m_pPlasmaGrenadeIcon;
	m_pPlasmaGrenadeIcon = nullptr;

	delete m_pNrOfGrenadesText;
	m_pNrOfGrenadesText = nullptr;
}

void HUD::Draw() const
{
	// shield
	m_pShieldBackGroundFill->Draw(m_ShieldBackGroundFillDstRect);
	m_pShieldFill->Draw(m_ShieldFillDstRect, m_ShieldSourceRect);
	if (m_ShieldLowTime < m_TimePerFlicker)
	{
		m_pShieldIcon->Draw(m_ShieldIconDstRect);
		m_pShieldOutline->Draw(m_ShieldOutlineDstRect);
	}
	else
	{
		m_pShieldIconLow->Draw(m_ShieldIconDstRect);
		m_pShieldOutlineLow->Draw(m_ShieldOutlineDstRect);
	}

	// health
	if (m_DrawLowHealthIcon)
	{
		m_pHealthIconLow->Draw(m_HealthIconDstRect);
	}
	else
	{
		m_pHealthIcon->Draw(m_HealthIconDstRect);
	}

	Rectf healthTexturePosition{ m_HealthTextureDstRect };
	const float distanceBetweenTextures{ 8.5f };

	for (int i{}; i < m_NrOfHealthTextures; ++i)
	{
		m_pHealthTexture->Draw(healthTexturePosition);
		healthTexturePosition.left -= (distanceBetweenTextures + m_pHealthTexture->GetWidth());
	}

	// bullets
	m_pUITopBack->Draw(m_UITopBackDstRect);
	m_pNrOfBulletsLeft->Draw(Point2f(m_UITopBackDstRect.left + 30, m_UITopBackDstRect.bottom + 3));
	if (m_DrawPlasmaWeaponUI)
	{
		m_pPlasmaWeaponUI->Draw(m_PlasmaWeaponUIDstRect);
		m_pUIBattery->Draw(m_BulletTypeDstRect);
		m_UIPlasmaBar->Draw(m_UIPlasmaBarDstRect, m_UIPlasmaBarSourceRect);
	}
	else
	{
		m_pUIBullet->Draw(m_BulletTypeDstRect);
	}

	DrawBulletsInMagUI(m_NrOfTotalBulletsPerMag, m_NrOfBulletsInMag);

	// grenades
	DrawGrenadeUI();

}

void HUD::Update(const Avatar& avatar, float elapsedSec)
{
	if (m_ShieldPercent <= 0.01f)
	{
		m_ShieldLowTime += elapsedSec;
	}
	else
	{
		m_ShieldLowTime = 0;
	}

	if (m_ShieldLowTime > m_TimePerFlicker * 2)
	{
		m_ShieldLowTime = 0;
	}


	UpdateShieldHUD(avatar.GetShield());
	UpdateHealthHUD(avatar.GetHealth());
	UpdateNrOfTotBullets(avatar.GetWeapon()->GetTotBulletsLeft());

	m_NrOfBulletsInMag = avatar.GetWeapon()->GetBulletsLeft();
	m_NrOfTotalBulletsPerMag = avatar.GetWeapon()->GetMaxBulletsInMag();

	if(dynamic_cast<PlasmaPistol*>(avatar.GetWeapon()) or dynamic_cast<PlasmaRifle*>(avatar.GetWeapon()))
	{
		m_DrawPlasmaWeaponUI = true;
		UpdatePlasmaBar(avatar.GetWeapon()->GetMaxCooldownTime(), avatar.GetWeapon()->GetCurrentCooldownTime());
	}
	else
	{
		m_DrawPlasmaWeaponUI = false;
	}

	if (dynamic_cast<AutoRifle*>(avatar.GetWeapon()))
	{
		m_DrawAutoRifleUI = true;
		m_DrawNeedlerUI = false;
	}
	else if (dynamic_cast<Needler*>(avatar.GetWeapon()))
	{
		m_DrawAutoRifleUI = false;
		m_DrawNeedlerUI	= true;
	}
	else
	{
		m_DrawAutoRifleUI = false;
		m_DrawNeedlerUI = false;
	}

	if (avatar.GetNrOfFragGrenades() > 0)
	{
		m_DrawFragUI = true;
		UpdateGrenadeText(avatar.GetNrOfFragGrenades());
	}
	else if(avatar.GetNrOfPlasmaGrenades() > 0)
	{
		m_DrawFragUI = false;
		UpdateGrenadeText(avatar.GetNrOfPlasmaGrenades());
	}
	else
	{
		m_DrawFragUI = true;
		UpdateGrenadeText(avatar.GetNrOfFragGrenades());
	}
}

void HUD::UpdateShieldHUD(float shield)
{
	m_ShieldPercent = shield / 100.f;
	if (m_ShieldPercent == 0)
	{
		m_ShieldPercent = 0.01f; // 1%
	}
	m_ShieldSourceRect.width = m_pShieldFill->GetWidth() * m_ShieldPercent;
	m_ShieldSourceRect.left = m_pShieldFill->GetWidth() - m_ShieldSourceRect.width;

	m_ShieldFillDstRect.width = m_ShieldOutlineDstRect.width * m_ShieldPercent;
	m_ShieldFillDstRect.left = m_ShieldOutlineDstRect.width + m_ShieldOutlineDstRect.left - m_ShieldFillDstRect.width;
}

void HUD::UpdateHealthHUD(float health)
{
	m_DrawLowHealthIcon = false;
	float healthPerBlock{ 100.f / float(m_MaxNrOfHealthTextures) };
	if (health <= healthPerBlock)
	{
		m_NrOfHealthTextures = 1;
		m_DrawLowHealthIcon = true;
	}
	else if (health <= healthPerBlock * 2)
	{
		m_NrOfHealthTextures = 2;
	}
	else if (health <= healthPerBlock * 3)
	{
		m_NrOfHealthTextures = 3;
	}
	else if (health <= healthPerBlock * 4)
	{
		m_NrOfHealthTextures = 4;
	}
	else if (health <= healthPerBlock * 5)
	{
		m_NrOfHealthTextures = 5;
	}
	else if (health <= healthPerBlock * 6)
	{
		m_NrOfHealthTextures = 6;
	}
	else if (health <= healthPerBlock * 7)
	{
		m_NrOfHealthTextures = 7;
	}
	else if (health <= healthPerBlock * 8)
	{
		m_NrOfHealthTextures = 8;
	}
	if (health == 0)
	{
		m_NrOfHealthTextures = 0;
	}
}

void HUD::UpdateNrOfTotBullets(int nrOfTotBullets)
{
	if(nrOfTotBullets != m_NrOfBulletsLeft)
	{
		m_NrOfBulletsLeft = nrOfTotBullets;
		delete m_pNrOfBulletsLeft;
		m_pNrOfBulletsLeft = nullptr;
		m_pNrOfBulletsLeft = new Texture(std::to_string(nrOfTotBullets), "Textures/Pixeboy-z8XGD.ttf", m_FontSize * m_Scale, Color4f{ 0.5f, .7f, 1.f, 1 });
	}
}

void HUD::DrawGrenadeUI() const
{
	if (m_DrawFragUI)
	{
		m_pFragGrenadeIcon->Draw(m_GrenadeIconDstRect);
	}
	else
	{
		m_pPlasmaGrenadeIcon->Draw(m_GrenadeIconDstRect);
	}

	m_pNrOfGrenadesText->Draw(Point2f{ m_GrenadeIconDstRect.left - 25, m_GrenadeIconDstRect.bottom - 3 });
}

void HUD::UpdateGrenadeText(int nrOfGrenades)
{
	if (nrOfGrenades != m_NrOfGrenades)
	{
		m_NrOfGrenades = nrOfGrenades;
		delete m_pNrOfGrenadesText;
		m_pNrOfGrenadesText = nullptr;
		m_pNrOfGrenadesText = new Texture(std::to_string(nrOfGrenades), "Textures/Pixeboy-z8XGD.ttf", m_FontSize * m_Scale, Color4f{ 0.5f, .7f, 1.f, 1 });
	}
}

void HUD::DrawBulletsInMagUI(int maxBullets, int bulletsInMag) const
{
	int nrOfBulletsPerLine{ 20 };
	int rowCounter{};

	if (m_DrawAutoRifleUI)
	{
		float verticalSpacing{ 3 };
		float horizontalSpacing{ 0 };
		Rectf dstRect{ m_UIAutoRifleDstRect };

		for (int idx{}; idx < maxBullets; ++idx)
		{

			if (idx % 20 == 0 and idx > 1)
			{
				++rowCounter;
				dstRect.left = m_UIAutoRifleDstRect.left - (m_UIAutoRifleDstRect.width * rowCounter);
				dstRect.bottom -= m_UIAutoRifleDstRect.height + verticalSpacing;
			}

			if (idx < bulletsInMag)
			{
				m_UIAutoRifleFull->Draw(dstRect);
			}
			else
			{
				m_UIAutoRifleEmpty->Draw(dstRect);
			}
			dstRect.left += m_UIAutoRifleDstRect.width + horizontalSpacing;
		}
		rowCounter = 0;
	}

	if (m_DrawNeedlerUI)
	{
		float verticalSpacing{ 3 };
		float horizontalSpacing{ -6 };
		Rectf dstRect{ m_UINeedlerDstRect };
		for (int idx{}; idx < maxBullets; ++idx)
		{
			if (m_DrawNeedlerUI)
			{
				if (idx % nrOfBulletsPerLine == 0 and idx > 1)
				{
					dstRect.left = m_UINeedlerDstRect.left;
					dstRect.bottom -= m_UINeedlerDstRect.height + verticalSpacing;
				}

				if (idx < bulletsInMag)
				{
					m_UINeedlerFull->Draw(dstRect);
				}
				else
				{
					m_UINeedlerEmpty->Draw(dstRect);
				}
				dstRect.left += m_UINeedlerDstRect.width + horizontalSpacing;
			}
		}
	}
}

void HUD::UpdatePlasmaBar(float max, float current)
{
	float precentage{ current / max };
	if (precentage == 0)
	{
		precentage = 0.0001f; // 1%
	}
	m_UIPlasmaBarDstRect.width = m_UIPlasmaBar->GetWidth() * m_Scale * precentage;
	m_UIPlasmaBarSourceRect.width = m_UIPlasmaBar->GetWidth() * precentage;
}
