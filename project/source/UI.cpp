#include "UI.h"

UI& UI::getInstance()
{
	static UI instance;
	return instance;
}

void UI::initialize(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(d3d::device, d3d::devcon);
	ImGui::StyleColorsDark();
}

void UI::render()
{
	renderImgui();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool UI::updateWindowEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
}

UI::Event UI::readEvent()
{
	if (eventsBuffer.size() > 0)
	{
		UI::Event event = eventsBuffer.front();
		eventsBuffer.pop();
		return event;
	}
	else
	{
		return UI::Event(UI::Event::None);
	}
}

bool UI::eventsBufferIsEmpty()
{
	return eventsBuffer.empty();
}

void UI::renderImgui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	renderMenuImgui();
	renderLogImgui();

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UI::renderLogImgui()
{
	ImGuiLog::getInstance().draw("Log");
}

void UI::renderMenuImgui()
{
	ImGui::Begin("Menu");
	if (ImGui::CollapsingHeader("Scene")) renderSceneSettingsImgui();
	if (ImGui::CollapsingHeader("View")) renderViewSettingsImgui();
	if (ImGui::CollapsingHeader("PostProcces")) renderPostProcessSettingsImgui();
	if (ImGui::CollapsingHeader("Help")) renderHelpImgui();
	ImGui::End();
}

void UI::renderSceneSettingsImgui()
{
	ImGui::InputText("scene_file_path", sceneFilePath, ARRAYSIZE(sceneFilePath));
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		eventsBuffer.push(UI::Event(UI::Event::LoadScene));
	}

	if (ImGui::TreeNode("main opaque instance"))
	{
		auto& perModelArr = graphics::MeshSystem::getInstance().getOpaqueInstances().getPerModelArr();

		if (ImGui::CollapsingHeader("shaders"))
		{
			renderPipelineShadersImgui(graphics::MeshSystem::getInstance().getOpaqueInstances().getPipelineShaders());
		}
		if (ImGui::CollapsingHeader("per_models_arr"))
		{
			renderScenePerModelsArrImgui(perModelArr);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("instances"))
	{
		renderSceneInstancesImgui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("light system"))
	{
		renderSceneLightSystemImgui();
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("particle system"))
	{
		renderSceneParticleSystemImgui();
		ImGui::TreePop();
	}

}

void UI::renderSceneParticleSystemImgui()
{
	const std::vector<graphics::Emmiter>& emmtiers = graphics::ParticleSystem::getInstance().getEmmiters();

	ImGui::Text("emmtiers list:");

	for (uint32 i = 0; i < emmtiers.size(); ++i)
	{
		const std::string& collapsingHeaderTitle = "emmiter (" + std::to_string(i) + ")";

		if (ImGui::TreeNode(collapsingHeaderTitle.c_str()))
		{
			std::string msg = "";
			msg += "position: " + to_string(utils::SolidVector<graphics::EmmiterPosition>::getInstance()[emmtiers[i].getPositionID()]) + "\n";
			msg += "particle color: " + to_string(emmtiers[i].getSpawnedParticleColor()) + "\n";
			msg += "particle size: " + to_string(emmtiers[i].getSpawnParticleSize()) + "\n";
			msg += "particle speed: " + to_string(emmtiers[i].getSpawnParticleSpeed()) + "\n";
			msg += "particle spawn per second: " + std::to_string(emmtiers[i].getSpawnRatePerSecond()) + "\n";
			msg += "spawn sphere radius: " + std::to_string(emmtiers[i].getSpawnCircleRadius()) + "\n";
			msg += "max particel num: " + std::to_string(emmtiers[i].getMaxParticlesNum());
			ImGui::Text(msg.c_str());
			ImGui::TreePop();
		}
	}

	ImGui::Text("other:");

	if (ImGui::TreeNode("Create new emmtier node"))
	{
		ImGui::InputFloat3("position", &newEmmiter.position.x);
		ImGui::InputFloat4("particle color", &newEmmiter.particleColor.x);
		ImGui::ColorEdit4("particle color", &newEmmiter.particleColor.x);
		ImGui::InputFloat2("particle size", &newEmmiter.particleSize.x);
		ImGui::InputFloat3("particle speed", &newEmmiter.particleSpeed.x);
		ImGui::InputFloat("particle spawn per second", &newEmmiter.particleSpawnRatePerSecond);
		ImGui::InputFloat("spawn sphere radius", &newEmmiter.spawnSphereRadius);
		ImGui::InputInt("max particel num", &newEmmiter.maxParticleNum);

		if (ImGui::Button("Spawn new emmiter"))
			eventsBuffer.push(UI::Event::SpawnEmmiter);

		ImGui::TreePop();
	}
}

void UI::renderViewSettingsImgui()
{
	if (ImGui::Checkbox("Scene shaders", &viewSceneShaders))
	{
		eventsBuffer.push(UI::Event(UI::Event::ViewShadersCheckbox));
	}
	if (ImGui::Checkbox("Normal visualization shaders", &viewNormalShaders))
	{
		eventsBuffer.push(UI::Event(UI::Event::ViewShadersCheckbox));
	}
	if (ImGui::Checkbox("Wireframe shaders", &viewWireframeShaders))
	{
		eventsBuffer.push(UI::Event(UI::Event::ViewShadersCheckbox));
	}
	
	static const char* filterTypes[] = { "Point", "Linear", "Anisotropic" };
	ImGui::Combo("Texture filter", &viewTextureFilter, filterTypes, IM_ARRAYSIZE(filterTypes));

	float32 sphereRadius = LightDebugger::getInstance().getPointLightSphereRadius();
	if (ImGui::InputFloat("point light sphere radius", &sphereRadius))
	{
		LightDebugger::getInstance().setPointLightSphereRadius(sphereRadius);
		LightDebugger::getInstance().update();
		eventsBuffer.push(UI::Event(UI::Event::LightDebuggerUpdated));
	}	
	
	if (ImGui::Checkbox("E_diff", &IBL_E_diff))
	{
		eventsBuffer.push(UI::Event(UI::Event::ViewShadersCheckbox));
	}
	if (ImGui::Checkbox("E_direct", &IBL_E_direct))
	{
		eventsBuffer.push(UI::Event(UI::Event::ViewShadersCheckbox));
	}
	if (ImGui::Checkbox("E_spec", &IBL_E_spec))
	{
		eventsBuffer.push(UI::Event(UI::Event::ViewShadersCheckbox));
	}
}

void UI::renderScenePerModelsArrImgui(std::vector<graphics::MeshSystem::OpaqueShadingGroup::PerModel>& perModelsArr)
{
	for (uint32 i = 0; i < perModelsArr.size(); ++i)
	{
		const std::string& collapsingHeaderTitle = "per_model id: " + std::to_string(i);

		if (ImGui::TreeNode(collapsingHeaderTitle.c_str()))
		{
			auto& perModel = perModelsArr[i];

			if (ImGui::CollapsingHeader("model"))
			{
				std::string modelInfoMsg;
				modelInfoMsg += "name: " + perModel.model->name + "\n";
				modelInfoMsg += "aabb:\n";
				modelInfoMsg += "\tmin: " + to_string(perModel.model->aabb.min) + "\n";
				modelInfoMsg += "\tmax : " + to_string(perModel.model->aabb.max) + "\n";
				modelInfoMsg += "meshes count: " + std::to_string(perModel.model->meshes.size());
				ImGui::Text(modelInfoMsg.c_str());

				for (uint32 k = 0; k < perModel.model->meshes.size(); ++k)
				{
					const std::string& meshCollapsingHeaderTitle = "mesh id: " + std::to_string(k);
					if (ImGui::TreeNode(meshCollapsingHeaderTitle.c_str()))
					{
						auto& mesh = perModel.model->meshes[k];

						std::string meshInfoMsg;
						meshInfoMsg += "name: " + mesh.name + "\n";
						meshInfoMsg += "aabb:\n";
						meshInfoMsg += "\tmin: " + to_string(mesh.aabb.min) + "\n";
						meshInfoMsg += "\tmax : " + to_string(mesh.aabb.max) + "\n";
						meshInfoMsg += "faces count: " + std::to_string(3 * mesh.triangles.size()) + "\n";
						meshInfoMsg += "vertices count: " + std::to_string(mesh.vertices.size()) + "\n";
						meshInfoMsg += "mesh_to_model matrix:\n";
						meshInfoMsg += to_string(mesh.meshToModel) + "\n";
						meshInfoMsg += "model_toMesh matrix:\n";
						meshInfoMsg += to_string(mesh.meshToModelInv) + "\n";

						ImGui::Text(meshInfoMsg.c_str());

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::CollapsingHeader("per_meshes_array"))
			{
				renderScenePerMeshesArrImgui(perModel.perMeshArr);
			}

			ImGui::TreePop();
		}
	}
}

void UI::renderScenePerMeshesArrImgui(std::vector<graphics::MeshSystem::OpaqueShadingGroup::PerMesh>& perMeshesArr)
{
	for (uint32 i = 0; i < perMeshesArr.size(); ++i)
	{
		const std::string& collapsingHeaderTitle = "per_mesh id: " + std::to_string(i);

		if (ImGui::TreeNode(collapsingHeaderTitle.c_str()))
		{
			auto& perMesh = perMeshesArr[i];
			renderScenePerMaterialsArrImgui(perMesh.perMaterialArr);

			ImGui::TreePop();
		}
	}
}

void UI::renderScenePerMaterialsArrImgui(std::vector<graphics::MeshSystem::OpaqueShadingGroup::PerMaterial>& perMaterialsArr)
{
	for (uint32 i = 0; i < perMaterialsArr.size(); ++i)
	{
		const std::string& collapsingHeaderTitle = "per_material id: " + std::to_string(i);

		if (ImGui::TreeNode(collapsingHeaderTitle.c_str()))
		{
			auto& perMaterial = perMaterialsArr[i];
			//TODO
			for (auto& instanceID : perMaterial.instancesID)
			{
				ImGui::InputInt("instance id", reinterpret_cast<int*>(&instanceID));
			}

			ImGui::TreePop();
		}
	}
}

void UI::renderSceneInstancesImgui()
{	
	ImGui::InputInt("Instance ID", &viewSelectedInstanceId);

	auto& instance = utils::SolidVector<graphics::MeshSystem::OpaqueInstanceGPU>::getInstance()[viewSelectedInstanceId];

	float32 instanceColor[4] = { instance.color.x, instance.color.y, instance.color.z, instance.color.w };

	if (ImGui::ColorEdit4("color", instanceColor))
	{
		instance.color.x = instanceColor[0];
		instance.color.y = instanceColor[1];
		instance.color.z = instanceColor[2];
		instance.color.w = instanceColor[3];

		graphics::MeshSystem::getInstance().getOpaqueInstances().updateInstanceBuffer();
	}
	std::string instanceInfoMsg;
	instanceInfoMsg += "model_to_world matrix:\n";
	instanceInfoMsg += to_string(instance.modelToWorld) + "\n";
	ImGui::Text(instanceInfoMsg.c_str());
}

void UI::renderSceneLightSystemImgui()
{
	bool mask = 0;
	auto lightsCBS = graphics::LightSystem::getInstance().getLightCBS();
	if (ImGui::TreeNode("directional lights"))
	{
		for (int32 i = 0; i < lightsCBS.directionalLightsSize.x; ++i)
		{
			if (ImGui::TreeNode(std::to_string(i).c_str()))
			{
				mask |= ImGui::InputFloat4("direction", &lightsCBS.directionalLightsCBS[i].direction.x);
				mask |= ImGui::ColorEdit3("intensity", &lightsCBS.directionalLightsCBS[i].intensity.x);
				mask |= ImGui::InputFloat4("intensity", &lightsCBS.directionalLightsCBS[i].intensity.x);
				mask |= ImGui::InputFloat("solid_angle", &lightsCBS.directionalLightsCBS[i].solidAngle.x);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("point lights"))
	{
		for (int32 i = 0; i < lightsCBS.pointLightsSize.x; ++i)
		{
			if (ImGui::TreeNode(std::to_string(i).c_str()))
			{
				mask |= ImGui::InputFloat3("position", &lightsCBS.pointLightCBS[i].position.x);
				mask |= ImGui::ColorEdit3("intensity", &lightsCBS.pointLightCBS[i].intensity.x);
				mask |= ImGui::InputFloat4("intensity", &lightsCBS.pointLightCBS[i].intensity.x);
				mask |= ImGui::InputFloat("radius", &lightsCBS.pointLightCBS[i].radius.x);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("spot lights"))
	{
		for (int32 i = 0; i < lightsCBS.spotLightsSize.x; ++i)
		{
			if (ImGui::TreeNode(std::to_string(i).c_str()))
			{
				mask |= ImGui::InputFloat3("position", &lightsCBS.spotLightsCBS[i].position.x);
				mask |= ImGui::ColorEdit3("intensity", &lightsCBS.spotLightsCBS[i].intensity.x);
				mask |= ImGui::InputFloat4("intensity", &lightsCBS.spotLightsCBS[i].intensity.x);
				mask |= ImGui::InputFloat3("direction", &lightsCBS.spotLightsCBS[i].direction.x);
				mask |= ImGui::InputFloat("radius", &lightsCBS.spotLightsCBS[i].radius.x);
				mask |= ImGui::SliderAngle("angle", &lightsCBS.spotLightsCBS[i].angle.x);
				ImGui::Checkbox("Detach the flashlight from the camera", &detachFlashlightFromCamera);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (mask)
	{
		graphics::LightSystem::getInstance().setLightCBS(lightsCBS);
		LightDebugger::getInstance().update();
		graphics::MeshSystem::getInstance().getDebugOpaqueInstances().updateInstanceBuffer();
	}
}

void UI::renderPostProcessSettingsImgui()
{
	renderPostProcessEV100SettingsImgui();
	
	float32 gamma = graphics::PostProcess::getInstance().getGamma();
	if (ImGui::InputFloat("gamma", &gamma))
		graphics::PostProcess::getInstance().setGamma(gamma);

	float32 qualityEdgeThreshold = graphics::PostProcess::getInstance().getQualityEdgeThreshold();
	if (ImGui::SliderFloat("quality edge threshold", &qualityEdgeThreshold, 0.063f, 0.333f))
		graphics::PostProcess::getInstance().setQualityEdgeThreshold(qualityEdgeThreshold);

	float32 qualityEdgeThresholdMin = graphics::PostProcess::getInstance().getQualityEdgeThresholdMin();
	if (ImGui::SliderFloat("quality edge threshold min", &qualityEdgeThresholdMin, 0.0312f, 0.0833f))
		graphics::PostProcess::getInstance().setQualityEdgeThresholdMin(qualityEdgeThresholdMin);

	float32 qualitySubpix = graphics::PostProcess::getInstance().getQualitySubpix();
	if (ImGui::SliderFloat("quality subpix", &qualitySubpix, 0.f, 1.f))
		graphics::PostProcess::getInstance().setQualitySubpix(qualitySubpix);
}

void UI::renderPostProcessEV100SettingsImgui()
{
	const char* elementsNames[] = {
		"Snow on a sunny day: 16",
		"Sunny day: 15",
		"Hazy, some clouds: 14",
		"Light clouds: 13",
		"Overcast, shady areas on a sunny day, sunriseand sunset: 12",
		"Just before sunrise and after sunset, the blue hour: 10",
		"Bright street light, bright indoor lighting: 8",
		"Indoor lighting.Bright window light: 6",
		"Dim window light: 3",
		"Dark morning before sunrise, dark evening after sunset: 0",
		"Moonlight from a full moon: -2",
		"Moonlight from a gibbous moon: -4",
		"Moonlight from quarter moon, bright aurora: -5",
		"Stars and starlight: -7",
		"Milky Way center: -10",
	};
	const int32 elementsValue[] = {
		16, 15, 14, 13, 12, 10, 8, 6, 3, 0, -2, -4, -5, -7, -10,
	};
	const int elementsCount = ARRAYSIZE(elementsNames);
	const char* elementName = (postprocessEv100Index >= 0 && postprocessEv100Index < elementsCount) ? elementsNames[postprocessEv100Index] : "Unknown";
	if (ImGui::SliderInt("ev100 presets", &postprocessEv100Index, 0, elementsCount - 1, elementName))
	{
		graphics::PostProcess::getInstance().setEV100(elementsValue[postprocessEv100Index]);
	}

	float32 ev100 = graphics::PostProcess::getInstance().getEV100();
	if (ImGui::InputFloat("ev100 custom", &ev100))
	{
		graphics::PostProcess::getInstance().setEV100(ev100);
	}
}

void UI::renderPipelineShadersImgui(const graphics::PipelineShaders& pipelineShaders)
{
	std::string shaderInfoMsg;
	shaderInfoMsg += "vertex shader: \"" + utils::StringConverter::stringFromWide(pipelineShaders.vertexShader.getShaderPath()) + "\"\n";
	shaderInfoMsg += "hull shader: \"" + utils::StringConverter::stringFromWide(pipelineShaders.hullShader.getShaderPath()) + "\"\n";
	shaderInfoMsg += "domain shader: \"" + utils::StringConverter::stringFromWide(pipelineShaders.domainShader.getShaderPath()) + "\"\n";
	shaderInfoMsg += "geometry shader: \"" + utils::StringConverter::stringFromWide(pipelineShaders.geometryShader.getShaderPath()) + "\"\n";
	shaderInfoMsg += "pixel shader: \"" + utils::StringConverter::stringFromWide(pipelineShaders.pixelShader.getShaderPath()) + "\"";

	ImGui::Text(shaderInfoMsg.c_str());
}

void UI::renderHelpImgui()
{
	ImGui::Text("Spawn - N");
}

std::string UI::to_string(const DirectX::XMVECTOR& v)
{
	std::string s;
	s += std::to_string(DirectX::XMVectorGetX(v)) + ", ";
	s += std::to_string(DirectX::XMVectorGetY(v)) + ", ";
	s += std::to_string(DirectX::XMVectorGetW(v)) + ", ";
	s += std::to_string(DirectX::XMVectorGetZ(v));
	return s;
}

std::string UI::to_string(const DirectX::XMFLOAT4X4& f)
{
	std::string s;
	s += "\t" + std::to_string(f._11) + ", " + std::to_string(f._12) + ", " + std::to_string(f._13) + ", " + std::to_string(f._14) + "\n";
	s += "\t" + std::to_string(f._21) + ", " + std::to_string(f._22) + ", " + std::to_string(f._23) + ", " + std::to_string(f._24) + "\n";
	s += "\t" + std::to_string(f._31) + ", " + std::to_string(f._32) + ", " + std::to_string(f._33) + ", " + std::to_string(f._34) + "\n";
	s += "\t" + std::to_string(f._41) + ", " + std::to_string(f._42) + ", " + std::to_string(f._43) + ", " + std::to_string(f._44);
	return s;
}

const char* UI::getSceneFilePath() const
{
	return sceneFilePath;
}

bool UI::getViewSceneShaders() const
{
	return viewSceneShaders;
}

bool UI::getViewNormalShaders() const
{
	return viewNormalShaders;
}

bool UI::getViewWireframeShaders() const
{
	return viewWireframeShaders;
}

int32 UI::getViewTextureFilter() const
{
	return viewTextureFilter;
}

bool UI::getDetachFlashlightFromCamera() const
{
	return detachFlashlightFromCamera;
}

int32 UI::getPostprocessEv100Index() const
{
	return postprocessEv100Index;
}

bool UI::getIBLEDiff() const
{
	return IBL_E_diff;
}

bool UI::getIBLEDirect() const
{
	return IBL_E_direct;
}

bool UI::getIBLESpec() const
{
	return IBL_E_spec;
}

void UI::setViewSceneShaders(bool val)
{
	viewSceneShaders = true;
}

void UI::setViewNormalShaders(bool val)
{
	viewNormalShaders = val;
}

void UI::setViewWireframeShaders(bool val)
{
	viewWireframeShaders = val;
}

void UI::setViewTextureFilter(int32 val)
{
	viewTextureFilter = val;
}

void UI::setPostprocessEv100Index(int32 val)
{
	postprocessEv100Index = val;
}

const EmmiterUIData& UI::getNewEmmiterUIData() const
{
	return newEmmiter;
}