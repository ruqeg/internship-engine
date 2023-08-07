template<class T_Instance>
void MyUtils::insertInstanceIDInShadingGroup(
	graphics::ShadingGroup<T_Instance>& shadingGroup,
	const uint32 instanceID,
	const std::shared_ptr<graphics::Model>& model,
	const graphics::Material& material)
{
	using ShadingGroupPerModel = typename graphics::ShadingGroup<T_Instance>::PerModel;
	using ShadingGroupPerMesh = typename graphics::ShadingGroup<T_Instance>::PerMesh;
	using ShadingGroupPerMaterial = typename graphics::ShadingGroup<T_Instance>::PerMaterial;

	auto& perModelArr = shadingGroup.getPerModelArr();

	const auto perModelIt = std::find_if(perModelArr.begin(), perModelArr.end(), [model](const ShadingGroupPerModel& perModel) -> bool {
		return perModel.model == model;
		});

	const bool findModelByte = perModelIt != std::end(perModelArr);
	if (!findModelByte)
	{
		ShadingGroupPerMaterial perMaterial;
		perMaterial.material = material;
		perMaterial.instancesID = { instanceID };

		ShadingGroupPerModel newPerModel;
		newPerModel.model = model;
		newPerModel.perMeshArr = { { { perMaterial } } };

		perModelArr.push_back(newPerModel);

		return;
	}

	ShadingGroupPerMaterial* pPerMaterial = nullptr;
	const auto& perMeshIt = std::find_if(perModelIt->perMeshArr.begin(), perModelIt->perMeshArr.end(), [&material, &pPerMaterial](ShadingGroupPerMesh& perMesh) -> bool {
		for (auto& perMaterial : perMesh.perMaterialArr)
		{
			if (material == perMaterial.material)
			{
				pPerMaterial = &perMaterial;
				return true;
			}
		}
	return false;
		});

	bool findMaterialByte = perMeshIt != std::end(perModelIt->perMeshArr);

	if (!findMaterialByte)
	{
		ShadingGroupPerMaterial perMaterial;
		perMaterial.material = material;
		perMaterial.instancesID = { instanceID };
		perMeshIt->perMaterialArr.push_back(perMaterial);
	}

	pPerMaterial->instancesID.push_back(instanceID);
}

template<class T_Instance>
void MyUtils::insertModelInstanceIDInShadingGroup(
	graphics::ShadingGroup<T_Instance>& shadingGroup,
	const uint32 instanceID,
	const std::shared_ptr<graphics::Model>& model,
	const std::vector<graphics::Material>& perMeshMaterials)
{
	using ShadingGroupPerModel = typename graphics::ShadingGroup<T_Instance>::PerModel;
	using ShadingGroupPerMesh = typename graphics::ShadingGroup<T_Instance>::PerMesh;
	using ShadingGroupPerMaterial = typename graphics::ShadingGroup<T_Instance>::PerMaterial;
	
	auto& perModelArr = shadingGroup.getPerModelArr();
	
	const auto perModelIt = std::find_if(perModelArr.begin(), perModelArr.end(), [model](const ShadingGroupPerModel& perModel) -> bool {
		return perModel.model == model;
		});
	
	const bool findModelByte = perModelIt != std::end(perModelArr);
	if (!findModelByte)
	{
		ShadingGroupPerModel newPerModel;
		newPerModel.model = model;
		newPerModel.perMeshArr.resize(perMeshMaterials.size());
		for (uint32 meshIndex = 0u; meshIndex < perMeshMaterials.size(); ++meshIndex)
		{
			ShadingGroupPerMaterial perMaterial;
			perMaterial.material = perMeshMaterials[meshIndex];
			perMaterial.instancesID = { instanceID };

			newPerModel.perMeshArr[meshIndex].perMaterialArr = { perMaterial };
		}
	
		perModelArr.push_back(newPerModel);
	
		return;
	}
	
	ShadingGroupPerMaterial* pPerMaterial = nullptr;
	for (uint32 meshIndex = 0; meshIndex < perModelIt->perMeshArr.size(); ++meshIndex)
	{
		ShadingGroupPerMaterial perMaterial;
		perMaterial.material = perMeshMaterials[meshIndex];
		perMaterial.instancesID = { instanceID };

		perModelIt->perMeshArr[meshIndex].perMaterialArr = { perMaterial };
	}
}