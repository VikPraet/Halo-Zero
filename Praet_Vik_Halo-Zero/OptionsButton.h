#pragma once
#include "BaseButton.h"

class Menu;
class OptionsButton: public BaseButton
{
public:
	OptionsButton(const std::string& buttonText, const Point2f& centerPos);
	void OnClick(const Point2f& mousePos, Menu& menu);

private:
};