#include <engine/graphics/MeshSystem.h>

namespace graphics
{
void MeshSystem::render(
	const ShadingGroupRenderBindTarget& bindTargets,
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& depthStencilState)
{
	d3d::devcon->OMSetDepthStencilState(depthStencilState.Get(), 2);
	graphics::DeferredShading::getInstance().renderEmission(debugOpaqueInstances, bindTargets);
	d3d::devcon->OMSetDepthStencilState(depthStencilState.Get(), 1);
	graphics::DeferredShading::getInstance().renderOpaque(opaqueInstances, bindTargets);
	graphics::DeferredShading::getInstance().renderOpaque(dissoultionShadingGroup, bindTargets);
	graphics::DeferredShading::getInstance().renderOpaque(incierationShadingGroup, bindTargets);
}

void MeshSystem::render(
	const ShadingGroupRenderBindTarget& bindTargets,
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& depthStencilState,
	graphics::PipelineShaders& pipelineShader)
{
	d3d::devcon->OMSetDepthStencilState(depthStencilState.Get(), 1);
	opaqueInstances.render(bindTargets, pipelineShader);
	dissoultionShadingGroup.render(bindTargets, pipelineShader);
	incierationShadingGroup.render(bindTargets, pipelineShader);
	d3d::devcon->OMSetDepthStencilState(depthStencilState.Get(), 2);
	debugOpaqueInstances.render(bindTargets, pipelineShader);
}

void MeshSystem::renderDepth2D(const ShadingGroupRenderBindTarget& bindTargets)
{
	shadowMapRenderer.renderDepth2D(this->opaqueInstances, bindTargets);
}

void MeshSystem::renderDepthCubemap(const ShadingGroupRenderBindTarget& bindTargets)
{
	shadowMapRenderer.renderDepthCubemaps(this->opaqueInstances, bindTargets);
}

void MeshSystem::initialize()
{
	shadowMapRenderer.initialize();
	opaqueInstances.initialize();
	debugOpaqueInstances.initialize();
	dissoultionShadingGroup.initialize();
	incierationShadingGroup.initialize();
}

void MeshSystem::updateModelInstancesIDs()
{
	uint32 modelID = 0u;
	uint32 materialID = 0u;

	ModelInstanceID modelInstanceID;

	for (auto& perModel : opaqueInstances.getPerModelArr())
	{
		for (auto& perMesh : perModel.perMeshArr)
		{
			for (auto& perMaterial : perMesh.perMaterialArr)
			{
				for (auto& instanceID : perMaterial.instancesID)
				{
					MeshInstanceID newMeshInstanceID;
					newMeshInstanceID.instanceID = instanceID;
					newMeshInstanceID.materialID = materialID;
					newMeshInstanceID.shadingID = 0u; // !TODO shading groups as array
					newMeshInstanceID.modelID = modelID;
					modelInstanceID.meshInstanceIDs.push_back(newMeshInstanceID);

					uint32 objectID = newMeshInstanceID.instanceID + newMeshInstanceID.materialID + newMeshInstanceID.modelID + newMeshInstanceID.shadingID;
					utils::SolidVector<OpaqueInstanceGPU>::getInstance()[instanceID].objectID = objectID;
				}
				materialID++;
			}
		}
		modelID++;
	}

	modelInstanceIDs = { modelInstanceID };
}

void MeshSystem::remove(const MeshInstanceID& meshInstanceID)
{
	// !TODO shading group as array
	if (meshInstanceID.shadingID == 0)
	{
		for (auto& perMesh : opaqueInstances.getPerModelArr()[meshInstanceID.modelID].perMeshArr)
		{
			auto& instancesID = perMesh.perMaterialArr[meshInstanceID.materialID].instancesID;
			instancesID.erase(std::find(instancesID.begin(), instancesID.end(), meshInstanceID.instanceID));
		}

		utils::SolidVector<OpaqueInstanceGPU>::getInstance().erase(meshInstanceID.instanceID);
	}
}

void MeshSystem::remove(uint16 shadingID, const uint32 modelID, const std::vector<uint16> perMeshMaterialIDs, const uint32 instanceID)
{
	// TODOx2
	auto& shadingGroup = opaqueInstances;

	for (uint32 meshIndex = 0u; meshIndex < shadingGroup.getPerModelArr()[modelID].perMeshArr.size(); ++meshIndex)
	{
		auto& perMesh = opaqueInstances.getPerModelArr()[modelID].perMeshArr[meshIndex];
		auto& instancesID = perMesh.perMaterialArr[perMeshMaterialIDs[meshIndex]].instancesID;
		instancesID.erase(std::find(instancesID.begin(), instancesID.end(), instanceID));
	}
}

void MeshSystem::setOpaqueInstances(const OpaqueShadingGroup& opaquseInstances)
{
	this->opaqueInstances = opaqueInstances;
}

MeshSystem::OpaqueShadingGroup& MeshSystem::getOpaqueInstances()
{
	return opaqueInstances;
}

MeshSystem::OpaqueShadingGroup& MeshSystem::getDebugOpaqueInstances()
{
	return debugOpaqueInstances;
}

MeshSystem::DissolutionShadingGroup& MeshSystem::getDissolutionShadingGroup()
{
	return dissoultionShadingGroup;
}

MeshSystem::IncinerationShadingGroup& MeshSystem::getIncinerationShadingGroup()
{
	return incierationShadingGroup;
}

MeshSystem& MeshSystem::getInstance()
{
	static MeshSystem instance;
	return instance;
};

}