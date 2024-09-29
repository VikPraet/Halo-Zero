#pragma once
#include "BaseSetting.h"

class Texture;
class Controls : public BaseSetting
{
public:
	Controls(const std::string& buttonText, const Point2f& centerPos);
	~Controls();

	void DrawOnHover() const override;
	void OnClick(const Point2f& mousePos);

	bool GetShowControls() const;
	void SetShowControls(bool showControls);
private:
	Texture*	m_pControlsText;
	Rectf		m_ControlsTextDstRect;
	bool		m_ShowControls;
};