#pragma once
#include "Texture.h"

class bullet_impact_BigBlue
{
public:
	bullet_impact_BigBlue(const Point2f& position, float scale);
	~bullet_impact_BigBlue();

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

