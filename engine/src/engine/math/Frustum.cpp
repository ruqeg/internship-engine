#include <engine/math/Frustum.h>

namespace math
{

void Frustum::create(const Camera& camera)
{
	// Frustum vertices in clipped sphere
	constexpr std::array<glm::vec4, 8> clipedVertices =
	{
		glm::vec4{-1, -1, 0, 1},
		glm::vec4{-1,  1, 0, 1},
		glm::vec4{ 1,  1, 0, 1},
		glm::vec4{ 1, -1, 0, 1},
		glm::vec4{-1, -1, 1, 1},
		glm::vec4{-1,  1, 1, 1},
		glm::vec4{ 1,  1, 1, 1},
		glm::vec4{ 1, -1, 1, 1}
	};

	// Trasform frustum cliped vertices to world space
	for (int i = 0; i < 8; i++)
	{
		glm::vec4 worldVertices = camera.toMatInv() * clipedVertices[i];
		vertices[i] = worldVertices / worldVertices.w;
	}

	// Calculate nearHorizontal, nearVertical
	const glm::vec3 NearClipLeftBottomVertex = vertices[0];
	const glm::vec3 NearClipLeftTopVertex = vertices[1];
	const glm::vec3 NearClipRightTopVertex = vertices[2];
	nearHorizontal = NearClipRightTopVertex - NearClipLeftTopVertex;
	nearVertical = NearClipLeftTopVertex - NearClipLeftBottomVertex;
}

constexpr std::array<glm::vec3, 8> Frustum::getVertices() const
{
	return vertices;
}

const glm::vec3& Frustum::getNearVertical() const
{
	return nearVertical;
}

const glm::vec3& Frustum::getNearHorizontal() const
{
	return nearHorizontal;
}

const glm::vec3& Frustum::getNearLeftTop() const
{
	return vertices[1];
}

}