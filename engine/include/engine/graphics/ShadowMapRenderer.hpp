#pragma

#include <engine/directx/D3D.h>
#include <engine/graphics/PipelineShaders.h>
#include <engine/graphics/ShadingGroup.h>
#include <engine/utils/FileSystem.h>

namespace graphics
{

struct ShadowMap2DConstantBuffer
{
	static constexpr uint32 shaderRegister = 10u;
	XMFLOAT4X4 worldToLightView;
	XMFLOAT4X4 lightViewToClip;
	float texelWorldSize;
	XMFLOAT3 _pad1;
	XMFLOAT3 lightPosition;
	float _pad2;
};

struct ShadowMap3DConstantBuffer
{
	static constexpr uint32 shaderRegister = 10u;
	XMFLOAT4X4 worldToLightViewsPerFace[6];
	XMFLOAT4X4 lightViewToClip;
	float texelWorldSize;
	XMFLOAT3 _pad1;
	XMFLOAT3 lightPosition;
	float _pad2;
};

class ShadowMapRenderer
{
public:
	void initialize();
	template<class T_Instance>
	void renderDepth2D(graphics::ShadingGroup<T_Instance>& opaqueInstance, const ShadingGroupRenderBindTarget& bindTargets);
	template<class T_Instance>
	void renderDepthCubemaps(graphics::ShadingGroup<T_Instance>& opaqueInstance, const ShadingGroupRenderBindTarget& bindTargets);
private:
	void initializeShaders();
private:
	PipelineShaders m_shadowmap2DpipelineShaders;
	PipelineShaders m_shadowmap3DpipelineShaders;
};

#include <engine/graphics/ShadowMapRenderer.inl>

}