#pragma once

#include <engine/Engine.h>
#include <engine/math/RCamera.h>

class CameraController
{
public:
	bool updateMoving(const window::Keyboard& keyboard, float64 elapsed, math::RCamera& camera);
	bool updateRotating(const window::Mouse& mouse, const window::MouseEvent& me, const window::MousePoint& oldMousePoint, float64 elapsed, math::RCamera& camera);

	void setMovingSpeed(float32 speed);
	void setRotatingSpeed(float32 speed);
private:
	float32 movingSpeed;
	float32 rotatingSpeed;
};