template<class T_Instance>
void ShadingGroup<T_Instance>::updateInstanceBuffer()
{
	uint32 totalInstancesID = 0u;
	for (auto& perModel : m_perModelArr)
	{
		for (auto& perMesh : perModel.perMeshArr)
		{
			for (const auto& perMaterial : perMesh.perMaterialArr)
			{
				totalInstancesID += static_cast<uint32>(perMaterial.instancesID.size());
			}
		}
	}

	if (totalInstancesID == 0u)
	{
		return;
	}

	std::vector<T_Instance> instanceBufferData(totalInstancesID);

	uint32 copiedNum = 0u;
	for (const auto& perModel : m_perModelArr)
	{
		for (const auto& perMesh : perModel.perMeshArr)
		{
			for (const auto& perMaterial : perMesh.perMaterialArr)
			{
				for (const auto& instanceID : perMaterial.instancesID)
				{
					instanceBufferData[copiedNum++] = utils::SolidVector<T_Instance>::getInstance()[instanceID];
				}
			}
		}
	}

	m_instanceBuffer.initialize(instanceBufferData.data(), instanceBufferData.size());
}

template<class T_Instance>
void ShadingGroup<T_Instance>::initialize()
{
	m_meshCB.initialize();
	m_materialCB.initialize();
}

template<class T_Instance>
void ShadingGroup<T_Instance>::setPerModelArr(const std::vector<PerModel>& perModelArr)
{
	m_perModelArr = perModelArr;
}

template<class T_Instance>
void ShadingGroup<T_Instance>::render(const ShadingGroupRenderBindTarget& bindTargets)
{
	render(bindTargets, m_pipelineShaders);
}

template<class T_Instance>
void ShadingGroup<T_Instance>::render(const ShadingGroupRenderBindTarget& bindTargets, PipelineShaders& pipelineShaders)
{
	m_instanceBuffer.bind(1u, 1u, 0u);

	uint32 renderedInstances = 0u;

	pipelineShaders.bind();

	for (auto& perModel : m_perModelArr)
	{
		const std::shared_ptr<Model>& model = perModel.model;

		perModel.model->vertexBuffer.bind(0u, 1u, 0u);
		perModel.model->indexBuffer.bind(0u);

		for (uint32 meshIndex = 0u; meshIndex < perModel.perMeshArr.size(); ++meshIndex)
		{
			const Mesh& mesh = model->meshes[meshIndex];
			const Model::MeshRange& meshRange = model->ranges[meshIndex];

			m_meshCB.data.meshToModel = mesh.meshToModel;
			m_meshCB.applyChanges(bindTargets.meshStartSlot, bindTargets.meshMapTarget);

			for (const auto& perMaterial : perModel.perMeshArr[meshIndex].perMaterialArr)
			{
				if (perMaterial.instancesID.empty())
				{
					continue;
				}

				const auto& material = perMaterial.material;
				//materialCB.data = bindTargets;
				m_materialCB.applyChanges(bindTargets.materialStartSlot, bindTargets.materialMapTarget);

				if (material.albedoMap)
					d3d::devcon->PSSetShaderResources(bindTargets.albedoMapStartSlot, 1, material.albedoMap->shaderResourceView.GetAddressOf());
				if (material.metallicMap)
					d3d::devcon->PSSetShaderResources(bindTargets.metallicMapStartSlot, 1, material.metallicMap->shaderResourceView.GetAddressOf());
				if (material.roughnessMap)
					d3d::devcon->PSSetShaderResources(bindTargets.roughnessMapStartSlot, 1, material.roughnessMap->shaderResourceView.GetAddressOf());
				if (material.normalMap)
					d3d::devcon->PSSetShaderResources(bindTargets.normalMapStartSlot, 1, material.normalMap->shaderResourceView.GetAddressOf());

				uint32 numInstances = perMaterial.instancesID.size();
				d3d::devcon->DrawIndexedInstanced(meshRange.indexNum, numInstances, meshRange.indexOffset, meshRange.vertexOffset, renderedInstances);
				renderedInstances += numInstances;
			}
		}
	}
}

template<class T_Instance>
std::vector<class ShadingGroup<T_Instance>::PerModel>& ShadingGroup<T_Instance>::getPerModelArr()
{
	return m_perModelArr;
}

template<class T_Instance>
const std::vector<class ShadingGroup<T_Instance>::PerModel>& ShadingGroup<T_Instance>::getPerModelArr() const
{
	return m_perModelArr;
}

template<class T_Instance>
void ShadingGroup<T_Instance>::setPipelineShaders(const PipelineShaders& pipelineShaders)
{
	m_pipelineShaders = pipelineShaders;
}

template<class T_Instance>
const PipelineShaders& ShadingGroup<T_Instance>::getPipelineShaders() const
{
	return m_pipelineShaders;
}