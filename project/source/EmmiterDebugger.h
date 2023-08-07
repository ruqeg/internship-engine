#pragma once

#include <engine/graphics/MeshSystem.h>
#include <engine/graphics/ParticleSystem.h>
#include "MyUtils.h"

class EmmiterDebugger
{
private:
	using SolidVectorOpaqueInstance = utils::SolidVector<graphics::MeshSystem::OpaqueInstanceGPU>;
public:
	static EmmiterDebugger& getInstance();
public:
	void addDebugger(const std::shared_ptr<graphics::Model> model, const DirectX::XMVECTOR& color, float scale, utils::SolidVector<graphics::EmmiterPosition>::ID emmiterpositionID);
	void changeEmmiterPosition(SolidVectorOpaqueInstance::ID instanceID);
	bool isEmmiterInstanceID(SolidVectorOpaqueInstance::ID instanceID) const;
private:
	EmmiterDebugger() = default;
private:
	std::unordered_map<SolidVectorOpaqueInstance::ID, int32> instanceID_emmiterID_map;
};