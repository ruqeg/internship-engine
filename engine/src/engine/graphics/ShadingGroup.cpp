#include <engine/graphics/ShadingGroup.h>	

namespace graphics
{

ShadingGroupRenderBindTarget::ShadingGroupRenderBindTarget(
	UINT meshStartSlot,
	UINT meshMapTarget,
	UINT materialStartSlot,
	UINT materialMapTarget,
	UINT albedoMapStartSlot,
	UINT metallicMapStartSlot,
	UINT roughnessMapStartSlot,
	UINT normalMapStartSlot)
	:
	meshStartSlot(meshStartSlot),
	meshMapTarget(meshMapTarget),
	materialStartSlot(materialStartSlot),
	materialMapTarget(materialMapTarget),
	albedoMapStartSlot(albedoMapStartSlot),
	metallicMapStartSlot(metallicMapStartSlot),
	roughnessMapStartSlot(roughnessMapStartSlot),
	normalMapStartSlot(normalMapStartSlot)
{}

}