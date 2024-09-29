#pragma once
#include "BaseObject.h"

class Texture;
class PlasmaGrenadeObject: public BaseObject
{
public:
	PlasmaGrenadeObject(const Point2f& position, float scale);
	~PlasmaGrenadeObject();

	void Draw() const;
	void Update(Avatar*& avatar);

	void CleanUp();

	void SetSFXVolume(int volume);

private:
	static SoundEffect* m_pPickUpSound;

	static Texture* m_pTexture;
	static int instanceCounter;

	bool m_IsPickedUp;
	Rectf m_Shape;
};

