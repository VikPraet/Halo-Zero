#include "pch.h"
#include "Camera.h"

Camera::Camera(float width, float height) :
	m_Width{width},
	m_Heigth{height},
	m_LevelBoundaries{ },
	m_CameraPos	{ },
	m_DistanceFromBordes{ 500.f }
{

}

void Camera::SetLevelBoundaries(const Rectf& levelBoundaries)
{
	m_LevelBoundaries = levelBoundaries;
	m_CameraPos = Point2f{ 700.f - m_Width / 2, 800.f};
}

Point2f Camera::Translate(const Rectf& target) const
{
	Point2f cameraBottomLeft{ Track(target) };
	Clamp(cameraBottomLeft);

	glTranslatef(-cameraBottomLeft.x, -cameraBottomLeft.y, 0);
	return cameraBottomLeft;
}

void Camera::TranslateParallaxOne(const Rectf& target) const
{
	Point2f cameraBottomLeft{ Track(target) };
	Clamp(cameraBottomLeft);

	const float translateModifier{ 0.5f };
	glTranslatef(-cameraBottomLeft.x * translateModifier, -cameraBottomLeft.y * translateModifier, 0);
}

void Camera::TranslateParallaxTwo(const Rectf& target) const
{
	Point2f cameraBottomLeft{ Track(target) };
	Clamp(cameraBottomLeft);

	const float translateModifier{ 0.25f };
	glTranslatef(-cameraBottomLeft.x * translateModifier, -cameraBottomLeft.y * translateModifier, 0);
}

void Camera::Update(float elapsedSec, const Rectf& target)
{
	m_CameraPos = Track(target);
}

Point2f Camera::Track(const Rectf& target) const
{
	float centerX{target.left + (target.width / 2)};
	float centerY{ target.bottom + (target.height / 2) };

	Point2f cameraBottomLeft{ m_CameraPos };

	if (centerX > m_CameraPos.x + m_Width - m_DistanceFromBordes)
	{
		cameraBottomLeft.x = centerX - m_Width + m_DistanceFromBordes;
	}
	if (centerX < m_CameraPos.x + m_DistanceFromBordes)
	{
		cameraBottomLeft.x = centerX - m_DistanceFromBordes;
	}

	if (centerY < m_CameraPos.y + m_Heigth - m_DistanceFromBordes)
	{
		cameraBottomLeft.y = centerY - m_Heigth + m_DistanceFromBordes;
	}
	if (centerY > m_CameraPos.y + m_DistanceFromBordes)
	{
		cameraBottomLeft.y = centerY - m_DistanceFromBordes;
	}

	return Point2f{ cameraBottomLeft };
}

void Camera::Clamp(Point2f& bottomLeft) const
{
	if (bottomLeft.x < m_LevelBoundaries.left) {
		bottomLeft.x = m_LevelBoundaries.left;
	}
	if (bottomLeft.y < m_LevelBoundaries.bottom) {
		bottomLeft.y = m_LevelBoundaries.bottom;
	}
	if (bottomLeft.x + m_Width > m_LevelBoundaries.left + m_LevelBoundaries.width) {
		bottomLeft.x = m_LevelBoundaries.width - m_Width;
	}
	if (bottomLeft.y + m_Heigth > m_LevelBoundaries.bottom + m_LevelBoundaries.height) {
		bottomLeft.y = m_LevelBoundaries.height - m_Heigth;
	}
}