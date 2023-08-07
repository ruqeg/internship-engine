#pragma once

#include <engine/types.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace math
{

class Camera
{
public:
	void setWorldOffset(const glm::vec3& offset);
	void addWorldOffset(const glm::vec3& offset);
	void addRelativeOffset(const glm::vec3& offset);
	void setWorldAngles(const glm::vec3& angles);
	void addWorldAngles(const glm::vec3& angles);
	void addRelativeAngles(const glm::vec3& angles);
	void setPerspective(float32 fovy, float32 aspect, float32 near, float32 far);

	inline const glm::mat4x4& toMat() const;
	inline const glm::mat4x4& toMatInv() const;

	inline const glm::vec3& getPosition() const;
	inline const glm::vec3& getRotation() const;

private:
	void updateMainMat();
	inline glm::vec3 right() const;
	inline glm::vec3 top() const;
	inline glm::vec3 forward() const;

private:
	glm::vec3 position;
	glm::vec3 angles;

	glm::quat rotation;

	glm::mat4x4 perspectiveMat;
	glm::mat4x4 perspectiveMatInv;
	glm::mat4x4 viewMatInv;
	glm::mat4x4 mainMat;
	glm::mat4x4 mainMatInv;
};

#include <engine/math/Camera.inl>

}