#pragma once
#include "BaseButton.h"

class StartButton: public BaseButton
{
public:
	StartButton(const std::string& buttonText, const Point2f& centerPos);
	void OnClick(const Point2f& mousePos, bool& StartGame);

private:
};