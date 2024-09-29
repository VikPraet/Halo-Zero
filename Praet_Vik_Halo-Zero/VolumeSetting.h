#pragma once
#include "BaseSetting.h"

class Texture;
class VolumeSetting: public BaseSetting
{
public:
	VolumeSetting(const std::string& buttonText, const Point2f& centerPos);
	~VolumeSetting();

	void DrawOnHover() const override;
	void OnClick(const Point2f& mousePos);

	float GetMusicPercent() const;
	void SetVolumePercent(int volume);

	void CheckVolumePercent();

private:
	int		m_VolumePercent;
	int		m_VolumeIncrement;

	Texture* m_pVolumeArrowRight;
	Texture* m_pVolumeArrowLeft;
	Texture* m_pVolumePercentText;

	Rectf m_DecreaseArrowDstrect;
	Rectf m_DecreaseInteractRect;

	Rectf m_IncreaseArrowDstrect;
	Rectf m_IncreaseInteractRect;

	Rectf m_VolumePercentDstRect;

	void CheckMinMaxPercent();
};