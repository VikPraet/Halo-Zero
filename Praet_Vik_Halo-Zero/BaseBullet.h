#pragma once
#include "utils.h"
#include "Texture.h"

class BaseBullet
{
public:
	BaseBullet();
	virtual ~BaseBullet() = default;

	virtual void Draw() const = 0;
	virtual void Update(float elapsedSec) = 0;

	virtual Point2f GetCenterPos() const = 0;
	virtual Point2f GetLeftMiddlePos() const = 0;
	virtual Point2f GetRightMiddlePos() const = 0;
	virtual float GetBulletDamage() const = 0;

	virtual void CleanUp() = 0;

protected:
};