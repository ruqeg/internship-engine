#include <engine/math/Camera.h>

namespace math
{

void Camera::setPerspective(float32 fovy, float32 aspect, float32 near, float32 far)
{
	perspectiveMat = glm::perspective(fovy, aspect, near, far);
	perspectiveMatInv = glm::inverse(perspectiveMat);
	updateMainMat();
}

void Camera::setWorldOffset(const glm::vec3& offset)
{
	position = offset;
	updateMainMat();
}

void Camera::addWorldOffset(const glm::vec3& offset)
{
	position += offset;
	updateMainMat();
}

void Camera::addRelativeOffset(const glm::vec3& offset)
{
	position += offset.x * right() + offset.y * top() + offset.z * forward();
	updateMainMat();
}

void Camera::setWorldAngles(const glm::vec3& angles)
{
	rotation = glm::quat(angles.x, glm::vec3(1.f, 0.f, 0.f));
	rotation *= glm::quat(angles.y, glm::vec3(0.f, 1.f, 0.f));
	rotation *= glm::quat(angles.z, glm::vec3(0.f, 0.f, 1.f));
	rotation = glm::normalize(rotation);
	updateMainMat();
}

void Camera::addWorldAngles(const glm::vec3& angles)
{
	rotation *= glm::quat(angles.x, glm::vec3(1.f, 0.f, 0.f));
	rotation *= glm::quat(angles.y, glm::vec3(0.f, 1.f, 0.f));
	rotation *= glm::quat(angles.z, glm::vec3(0.f, 0.f, 1.f));
	rotation = glm::normalize(rotation);
	updateMainMat();
}

void Camera::addRelativeAngles(const glm::vec3& angles)
{
	rotation *= glm::quat(angles.x, right());
	rotation *= glm::quat(angles.y, top());
	rotation *= glm::quat(angles.z, forward());
	rotation = glm::normalize(rotation);
	updateMainMat();
}

void Camera::updateMainMat()
{
	viewMatInv = glm::translate(position) * glm::mat4_cast(rotation);
	mainMat = perspectiveMat * glm::inverse(viewMatInv);
	mainMatInv = viewMatInv * perspectiveMatInv;
}

}