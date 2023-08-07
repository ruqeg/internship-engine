template<class T_Instance>
void ShadowMapRenderer::renderDepth2D(graphics::ShadingGroup<T_Instance>& opaqueInstance, const ShadingGroupRenderBindTarget& bindTargets)
{
	opaqueInstance.render(bindTargets, m_shadowmap2DpipelineShaders);
}

template<class T_Instance>
void ShadowMapRenderer::renderDepthCubemaps(graphics::ShadingGroup<T_Instance>& opaqueInstance, const ShadingGroupRenderBindTarget& bindTargets)
{
	opaqueInstance.render(bindTargets, m_shadowmap3DpipelineShaders);
}