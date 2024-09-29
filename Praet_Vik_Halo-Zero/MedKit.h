#pragma once
#include "BaseObject.h"

class Texture;
class MedKit: public BaseObject
{
public:
	MedKit(const Point2f& position, float scale);
	~MedKit();

	void Draw() const;
	void Update(Avatar*& avatar);

	void CleanUp();

	void SetSFXVolume(int volume);

private:
	static SoundEffect* m_pPickUpSound;

	static Texture* m_pTexture;
	static int instanceCounter;

	float m_HealthValue;
	bool m_IsPickedUp;
	Rectf m_Shape;
};
