#include "CameraController.h"

bool CameraController::updateMoving(const window::Keyboard& keyboard, float64 elapsed, math::RCamera& camera)
{
	bool cameraTransformChanged = false;

	if (keyboard.keyIsPressed('W'))
	{
		camera.addPosition(camera.getForwardVector() * movingSpeed * static_cast<float32>(elapsed));
		cameraTransformChanged = true;
	}
	if (keyboard.keyIsPressed('S'))
	{
		camera.addPosition(-camera.getForwardVector() * movingSpeed * static_cast<float32>(elapsed));
		cameraTransformChanged = true;
	}
	if (keyboard.keyIsPressed('D'))
	{
		camera.addPosition(camera.getRightVector() * movingSpeed * static_cast<float32>(elapsed));
		cameraTransformChanged = true;
	}
	if (keyboard.keyIsPressed('A'))
	{
		camera.addPosition(-camera.getRightVector() * movingSpeed * static_cast<float32>(elapsed));
		cameraTransformChanged = true;
	}
	if (keyboard.keyIsPressed(VK_SPACE))
	{
		camera.addPosition(camera.getTopVector() * movingSpeed * static_cast<float32>(elapsed));
		cameraTransformChanged = true;
	}
	if (keyboard.keyIsPressed('Z'))
	{
		camera.addPosition(-camera.getTopVector() * movingSpeed * static_cast<float32>(elapsed));
		cameraTransformChanged = true;
	}

	return cameraTransformChanged;
}

bool CameraController::updateRotating(const window::Mouse& mouse, const window::MouseEvent& me, const window::MousePoint& oldMousePoint, float64 elapsed, math::RCamera& camera)
{
	if (me.getType() == window::MouseEvent::EventType::Move)
	{
		if (mouse.isRightDown())
		{
			camera.addRotation(
				rotatingSpeed * (me.getPosY() - oldMousePoint.y) * static_cast<float32>(elapsed),
				rotatingSpeed * (me.getPosX() - oldMousePoint.x) * static_cast<float32>(elapsed),
				0);
			return true;
		}
	}
	return false;
}

void CameraController::setMovingSpeed(float32 speed)
{
	movingSpeed = speed;
}

void CameraController::setRotatingSpeed(float32 speed)
{
	rotatingSpeed = speed;
}