#pragma once

#include <engine/graphics/MeshSystem.h>
#include <engine/graphics/ConstantBuffer.h>
#include <engine/graphics/LightSystem.h>
#include <engine/utils/FileSystem.h>
#include <engine/utils/ModelManager.h>

#include <memory>

class LightDebugger
{
protected:
	using SolidVectorOpaqueInstance = utils::SolidVector<graphics::MeshSystem::OpaqueInstanceGPU>;
public:
	void initialize(const std::shared_ptr<graphics::Model>& pointlightModel = std::make_shared<graphics::Model>());
	void instertLightSystemInstances();
	void update();
	const graphics::MeshSystem::OpaqueInstanceGPU& instanceFromPointlightCBS(const graphics::PointLightsCBS& pointLightCBS);
	void changePointlight(SolidVectorOpaqueInstance::ID instanceID);
	bool isPointlightInstanceID(SolidVectorOpaqueInstance::ID instanceID);
	void setPointLightModel(const std::shared_ptr<graphics::Model>& model);
	void setPointLightSphereRadius(const float32 sphereRadius);
	float32 getPointLightSphereRadius() const;
public:
	static LightDebugger& getInstance();
private:
	LightDebugger() = default;
	LightDebugger(LightDebugger&) = delete;
	LightDebugger& operator=(LightDebugger&) = delete;
private:
	std::unordered_map<SolidVectorOpaqueInstance::ID, int32> instanceIdLightIndexMap;
	float32 pointLightSphereRadius;
};