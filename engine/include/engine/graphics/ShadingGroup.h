/*
 * CBS - Constant Buffer Struct
 * CB - Constant Buffer
 */

#pragma once

#include <engine/graphics/ConstantBuffer.h>
#include <engine/graphics/VertexBuffer.h>
#include <engine/graphics/IndexBuffer.h>
#include <engine/math/RTransform.h>
#include <engine/graphics/Model.h>
#include <engine/graphics/Material.h>
#include <engine/utils/SolidVector.h>
#include <engine/graphics/PipelineShaders.h>

#include <optional>

namespace graphics
{

struct MeshCBS
{
	XMFLOAT4X4 meshToModel;
};

struct MaterialCBS
{
};

struct ShadingGroupRenderBindTarget
{
	UINT meshStartSlot;
	UINT meshMapTarget;
	UINT materialStartSlot;
	UINT materialMapTarget;
	UINT albedoMapStartSlot;
	UINT metallicMapStartSlot;
	UINT roughnessMapStartSlot;
	UINT normalMapStartSlot;


	ShadingGroupRenderBindTarget(
		UINT meshStartSlot,
		UINT meshMapTarget,
		UINT materialStartSlot,
		UINT materialMapTarget,
		UINT albedoMapStartSlot,
		UINT metallicMapStartSlot,
		UINT roughnessMapStartSlot,
		UINT normalMapStartSlot);
};

template<class T_Instance>
class ShadingGroup
{
public:
	struct PerMaterial
	{
		Material material;
		std::vector<uint32> instancesID;
	};
	struct PerMesh
	{
		std::vector<PerMaterial> perMaterialArr;
	};
	struct PerModel
	{
		std::shared_ptr<Model> model;
		std::vector<PerMesh> perMeshArr;
	};

public:
	void updateInstanceBuffer();
	void render(const ShadingGroupRenderBindTarget& bindTargets);
	void render(const ShadingGroupRenderBindTarget& bindTargets, PipelineShaders& pipelineShaders);

	void setPipelineShaders(const PipelineShaders& pipelineShaders);
	const PipelineShaders& getPipelineShaders() const;

	void setPerModelArr(const std::vector<PerModel>& perModelArr);
	std::vector<PerModel>& getPerModelArr();
	const std::vector<PerModel>& getPerModelArr() const;

private:
	void initialize();

private:
	PipelineShaders m_pipelineShaders;
	std::vector<PerModel> m_perModelArr;
	graphics::VertexBuffer<T_Instance> m_instanceBuffer;

	graphics::ConstantBuffer<MeshCBS> m_meshCB;
	graphics::ConstantBuffer<MaterialCBS> m_materialCB;

	friend class MeshSystem;
};

#include <engine/graphics/ShadingGroup.inl>

}