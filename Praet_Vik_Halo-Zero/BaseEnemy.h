#pragma once
#include "SoundEffect.h"
#include "utils.h"
#include "BaseBullet.h"
#include "Level.h"
#include "BaseObject.h"

class Avatar;
class BaseWeapon;
class Marine;
class BaseEnemy
{
public:
	BaseEnemy();
	~BaseEnemy();

	virtual void Update(float elapsedSec, Avatar*& avatar, std::vector<Marine*>& marines, std::vector<BaseObject*>& objects, Level& level) = 0;
	virtual void Draw() const = 0;
	virtual void DrawWeapon() const = 0;

	virtual Rectf GetShape() const = 0;
	virtual bool GetIsDead() const = 0;
	virtual void DecreaseHealth(float hitpoints) = 0;

	virtual BaseWeapon* GetWeapon() const = 0;
	virtual void SetWeapon(BaseWeapon*& weapon) = 0;

	virtual void CleanUp() = 0;
	virtual void SetSFXVolume(int volume) = 0;

protected:

};

