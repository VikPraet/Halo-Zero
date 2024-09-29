#pragma once
#include "BaseButton.h"

class QuitButton: public BaseButton
{
public:
	QuitButton(const std::string& buttonText, const Point2f& centerPos);
	void OnClick(const Point2f& mousePos, bool& quitGame);

private:
};

