#pragma once

#include <engine/math/RRay.h>
#include <engine/graphics/Model.h>
#include <engine/utils/SolidVector.h>
#include <engine/graphics/MeshSystem.h>

class IntersectionSystem
{
protected:
	using OpaqueInstance = graphics::MeshSystem::OpaqueInstanceGPU;
public:
	struct IntersectionSystemRecord
	{
		math::RIntersectionRecord intersectionRec;
		utils::SolidVector<OpaqueInstance>::ID instanceID;
		std::shared_ptr<graphics::Model> model;
		graphics::Material material;
		uint16 modelID;
		uint16 materialID;
	};
	struct ModelIntersectionSystemRecord
	{
		math::RIntersectionRecord intersectionRec;
		utils::SolidVector<OpaqueInstance>::ID instanceID;
		std::shared_ptr<graphics::Model> model;
		uint16 modelID;
		std::vector<uint16> perMeshMaterialIDs;
	};
	struct PerMaterial
	{
		std::vector<utils::SolidVector<OpaqueInstance>::ID> instanceID;
	};
	struct PerModel
	{
		std::shared_ptr<graphics::Model> model;
		std::vector<PerMaterial> perMaterials;
	};
public:
	static IntersectionSystem& getInstance();
public:
	void initialize();
	void update(const std::vector<PerModel>& perModelsArr);
	bool intersect(const math::RRay& ray, IntersectionSystemRecord& intersectionSystemRec);
	bool intersectModel(const math::RRay& ray, IntersectionSystemRecord& meshIntersectionSystemRec, ModelIntersectionSystemRecord& modelIntersectionSystemRec);
private:
	IntersectionSystem() = default;
private:
	std::vector<PerModel> perModelsArr;
};