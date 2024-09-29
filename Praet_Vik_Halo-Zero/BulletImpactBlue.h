#pragma once
#include "Texture.h"

class BulletImpactBlue
{
public:
	BulletImpactBlue(const Point2f& position, float scale);
	~BulletImpactBlue();

	void Draw() const;
	void Update(float elapsedSec);

private:
	static int instanceCounter;
	static Texture* m_pTexture;

	Point2f m_Position;
	float m_Scale;

	bool m_IsActive;
	bool m_hasLoopedAnim;

	int m_CurrentFrame;
	float m_AccuSec;
	const float SECONDS_PER_FRAME;
	const int NR_OF_FRAMES;
};

