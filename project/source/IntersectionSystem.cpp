#include "IntersectionSystem.h"

void IntersectionSystem::initialize()
{
	//TODO
}

IntersectionSystem& IntersectionSystem::getInstance()
{
	static IntersectionSystem instance;
	return instance;
}

void IntersectionSystem::update(const std::vector<PerModel>& perModelsArr)
{
	this->perModelsArr = perModelsArr;
}

bool IntersectionSystem::intersect(const math::RRay& ray, IntersectionSystemRecord& intersectionSystemRec)
{
	IntersectionSystemRecord nearestSysdRec;

	nearestSysdRec.intersectionRec = intersectionSystemRec.intersectionRec;

	uint32 modelID = 0u;
	uint32 materialID = 0u;

	bool intereseted = false;

	for (auto& perModel : perModelsArr)
	{
		const auto& model = perModel.model;

		for (auto& mesh : model->meshes)
		{
			for (auto& perMaterial : perModel.perMaterials)
			{
				for (auto& instanceID : perMaterial.instanceID)
				{
					math::RRay modelRay;
					{
						const DirectX::XMMATRIX instanceModelToWorldMat = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&utils::SolidVector<OpaqueInstance>::getInstance()[instanceID].modelToWorld));
						const DirectX::XMMATRIX instanceWorldToModelMat = DirectX::XMMatrixInverse(nullptr, instanceModelToWorldMat);
						const DirectX::XMVECTOR od = ray.origin + ray.direction;
						const DirectX::XMVECTOR modelRayEnd = DirectX::XMVector3Transform(od, instanceWorldToModelMat);
						modelRay.origin = DirectX::XMVector3Transform(ray.origin, instanceWorldToModelMat);
						modelRay.direction = modelRayEnd - modelRay.origin;
					}

					math::RIntersectionRecord modelIntersectionRec;
					if (mesh.trinagleOctree.intersect(modelRay, modelIntersectionRec))
					{
						if (modelIntersectionRec.t < nearestSysdRec.intersectionRec.t)
						{
							nearestSysdRec.model = model;
							nearestSysdRec.instanceID = instanceID;
							nearestSysdRec.intersectionRec = modelIntersectionRec;
							nearestSysdRec.materialID = materialID;
							nearestSysdRec.modelID = modelID;
							intereseted = true;
						}
					}
				}
				materialID++;
			}
		}
		modelID++;
	}

	if (intereseted)
	{
		const DirectX::XMMATRIX nearestModelToWorldMat = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&utils::SolidVector<OpaqueInstance>::getInstance()[nearestSysdRec.instanceID].modelToWorld));
		intersectionSystemRec.instanceID = nearestSysdRec.instanceID;
		intersectionSystemRec.intersectionRec.position = DirectX::XMVector3Transform(nearestSysdRec.intersectionRec.position, nearestModelToWorldMat);
		intersectionSystemRec.intersectionRec.t = nearestSysdRec.intersectionRec.t;
		intersectionSystemRec.intersectionRec.normal = DirectX::XMVector3Transform(nearestSysdRec.intersectionRec.normal, nearestModelToWorldMat);
		intersectionSystemRec.model = nearestSysdRec.model;
		intersectionSystemRec.materialID = nearestSysdRec.materialID;
		intersectionSystemRec.modelID = nearestSysdRec.modelID;
	}

	return intereseted;
}

bool IntersectionSystem::intersectModel(const math::RRay& ray, IntersectionSystemRecord& meshIntersectionSystemRec, ModelIntersectionSystemRecord& modelIntersectionSystemRec)
{
	if (!intersect(ray, meshIntersectionSystemRec))
		return false;

	ModelIntersectionSystemRecord nearestModelIntersectionSystemRec;
	nearestModelIntersectionSystemRec.instanceID = meshIntersectionSystemRec.instanceID;
	nearestModelIntersectionSystemRec.intersectionRec = meshIntersectionSystemRec.intersectionRec;
	nearestModelIntersectionSystemRec.model = meshIntersectionSystemRec.model;
	nearestModelIntersectionSystemRec.modelID = meshIntersectionSystemRec.modelID;

	uint32 modelIndex = nearestModelIntersectionSystemRec.modelID;
	const auto& perModel = perModelsArr[modelIndex];
	const auto& model = perModel.model;

	nearestModelIntersectionSystemRec.perMeshMaterialIDs.resize(model->meshes.size());

	for (uint32 meshIndex = 0u; meshIndex < model->meshes.size(); ++meshIndex)
	{
		auto& mesh = model->meshes[meshIndex];
		
		for (uint32 materialIndex = 0u; materialIndex < perModel.perMaterials.size(); ++materialIndex)
		{
			auto& perMaterial = perModel.perMaterials[materialIndex];

			const auto& it = std::find(perMaterial.instanceID.begin(), perMaterial.instanceID.end(), nearestModelIntersectionSystemRec.instanceID);
			if (it != perMaterial.instanceID.end())
			{
				nearestModelIntersectionSystemRec.perMeshMaterialIDs[meshIndex] = materialIndex;
				break;
			}
		}
	}
	
	modelIntersectionSystemRec = nearestModelIntersectionSystemRec;

	return true;
}