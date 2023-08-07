#include "EmmiterDebugger.h"

EmmiterDebugger& EmmiterDebugger::getInstance()
{
	static EmmiterDebugger instance;
	return instance;
}

void EmmiterDebugger::addDebugger(const std::shared_ptr<graphics::Model> model, const DirectX::XMVECTOR& color, float scale, utils::SolidVector<graphics::EmmiterPosition>::ID emmiterpositionID)
{
	graphics::EmmiterPosition position = utils::SolidVector<graphics::EmmiterPosition>::getInstance()[emmiterpositionID];
	
	graphics::MeshSystem::OpaqueInstanceGPU instance;
	DirectX::XMStoreFloat4(&instance.color, color);
	DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslation(
		DirectX::XMVectorGetX(position),
		DirectX::XMVectorGetY(position),
		DirectX::XMVectorGetZ(position));
	DirectX::XMMATRIX rotationMat = DirectX::XMMatrixScaling(scale, scale, scale);
	DirectX::XMStoreFloat4x4(&instance.modelToWorld, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(rotationMat, translationMat)));
	
	uint32 instanceID = SolidVectorOpaqueInstance::getInstance().insert(instance);

	MyUtils::insertInstanceIDInShadingGroup(
		graphics::MeshSystem::getInstance().getDebugOpaqueInstances(),
		instanceID,
		model,
		{});

	instanceID_emmiterID_map[instanceID] = emmiterpositionID;
}

void EmmiterDebugger::changeEmmiterPosition(SolidVectorOpaqueInstance::ID instanceID)
{
	auto& modelToWorld = SolidVectorOpaqueInstance::getInstance()[instanceID].modelToWorld;
	utils::SolidVector<graphics::EmmiterPosition>::getInstance()[instanceID_emmiterID_map[instanceID]] = DirectX::XMVectorSet(
		modelToWorld._14,
		modelToWorld._24,
		modelToWorld._34,
		modelToWorld._44);
}

bool EmmiterDebugger::isEmmiterInstanceID(SolidVectorOpaqueInstance::ID instanceID) const
{
	return instanceID_emmiterID_map.find(instanceID) != instanceID_emmiterID_map.end();
}
