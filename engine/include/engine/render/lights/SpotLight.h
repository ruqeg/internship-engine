#pragma once

#include <engine/render/lights/LightSource.h>

namespace render
{

struct SpotLight : public LightSource
{
public:
	bool illuminate(const glm::vec3& vertex, LightRecord& lightRec) const override;
	bool shadowBetweenVertexAndLight(float32 t) const override;
public:
	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 intensity;
	float32 length;
	float32 angle;
};

}