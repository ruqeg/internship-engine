#include <engine/math/Transform.h>

namespace math
{

Transform::Transform()
	: position(0.f), scale(1.f), rotation(0.f), modelMat(1.f), modelMatInv(1.f), rotationMat(1.f)
{}

void Transform::setPosition(const glm::vec3& position)
{
	this->position = position;
	updateModelMat();
}

void Transform::setRotation(const glm::vec3& rotation)
{
	this->rotation = rotation;
	updateRotationMat();
	updateModelMat();
}

void Transform::setScale(const glm::vec3& scale)
{
	this->scale = scale;
	updateModelMat();
}

void Transform::updateModelMat()
{
	modelMat = glm::translate(position) * rotationMat * glm::scale(scale);
	modelMatInv = glm::inverse(modelMat);
}

void Transform::updateRotationMat()
{
	rotationMat = glm::rotate(rotation.x, glm::vec3(1.f, 0.f, 0.f));
	rotationMat = glm::rotate(rotation.y, glm::vec3(0.f, 1.f, 0.f)) * rotationMat;
 	rotationMat = glm::rotate(rotation.z, glm::vec3(0.f, 0.f, 1.f)) * rotationMat;
	rotationMatInv = glm::inverse(rotationMat);
}

}