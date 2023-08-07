#pragma once

#include <engine/math/Camera.h>

#include <array>

namespace math
{

class Frustum
{
public:
	void create(const Camera& camera);

	constexpr std::array<glm::vec3, 8> getVertices() const;
	const glm::vec3& getNearVertical() const;
	const glm::vec3& getNearHorizontal() const;
	const glm::vec3& getNearLeftTop() const;

private:
	std::array<glm::vec3, 8> vertices;
	glm::vec3 nearHorizontal;
	glm::vec3 nearVertical;
};

}