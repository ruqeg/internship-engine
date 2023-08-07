#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <glm/geometric.hpp>
#include <glm/gtx/transform.hpp>

namespace math
{

class Transform
{
public:
	Transform();
	void setPosition(const glm::vec3& position);
	void setRotation(const glm::vec3& rotation);
	void setScale(const glm::vec3& scale);

	inline const glm::vec3& getPosition() const;
	inline const glm::vec3& getRotation() const;
	inline const glm::vec3& getScale() const;
	inline const glm::mat4x4& getRotationMat() const;
	inline const glm::mat4x4& getRotationMatInv() const;
	inline const glm::mat4x4& toMat() const;
	inline const glm::mat4x4& toMatInv() const;

private:
	void updateModelMat();
	void updateRotationMat();

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4x4 rotationMat;
	glm::mat4x4 rotationMatInv;

	glm::mat4x4 modelMat;
	glm::mat4x4 modelMatInv;
};

#include <engine/math/Transform.inl>

}