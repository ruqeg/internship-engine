#include "SceneLoader.h"

namespace nlohmann
{

void from_json(const json& j, XMFLOAT4& float4)
{
	auto arr = j;
	float4.x = j[0];
	float4.y = j[1];
	float4.z = j[2];
	float4.w = j[3];
}

}

SceneLoader& SceneLoader::getInstance()
{
	static SceneLoader instance;
	return instance;
}

nlohmann::json SceneLoader::createJsonFromFile(const std::string& path)
{
	std::ifstream inf(path);
	nlohmann::json j = nlohmann::json::parse(inf);
	return j;
}

void SceneLoader::loadBackroundColorFromJson(const nlohmann::json& j, std::array<float32, 4>& bgcolor)
{
	bgcolor = j["background-color"];
}

void SceneLoader::loadCameraFromJson(const nlohmann::json& j, float32 ratio, math::RCamera& camera)
{
	auto jsonCamera = j["camera"];
	auto jsonCameraPosition = j["camera"]["position"];
	auto jsonCameraRotation = j["camera"]["rotation"];
	camera.setPosition(jsonCameraPosition[0], jsonCameraPosition[1], jsonCameraPosition[2]);
	camera.setRotation(jsonCameraRotation[0], jsonCameraRotation[1], jsonCameraRotation[2]);
	camera.createProjectionMatrix(jsonCamera["fov"], ratio, jsonCamera["nearZ"], jsonCamera["farZ"]);
	camera.updateViewMatrix();
	camera.updateMainMatrix();
}

void SceneLoader::loadFullscreenTriangleFromJson(const nlohmann::json& j, FullscreenTriangle& fullscrenTriangle)
{
	auto jsonFullscreenTriangle = j["fullscreen-triangle"];
	const auto& shaderPath = jsonFullscreenTriangle["cube-texture"].get<std::string>();
	const auto& shaderPathW = utils::StringConverter::stringToWide(shaderPath);
	const auto& optionalTexture = utils::TextureManager::getInstance().getTexture(shaderPathW);
	if (!optionalTexture.has_value())
	{
		throw std::exception(("Can't load fullscreen triangle cubemap \"" + shaderPath + "\"").c_str());
	}
	fullscrenTriangle.setTexture(optionalTexture.value());
}

void SceneLoader::loadMeshSystemFromJson(const nlohmann::json& j)
{
	using OpaqueShadingGroup = graphics::MeshSystem::OpaqueShadingGroup;
	using OpaqueInstance = graphics::MeshSystem::OpaqueInstanceGPU;

	std::vector<OpaqueShadingGroup::PerModel> perShadersArr;
	
	std::unordered_map<uint32, utils::SolidVector<OpaqueInstance>::ID> instanceIdByIndex;
	
	constexpr uint32 OPAQUE_SHADING_GROUP_ID = 0u;
	
	uint32 instanceIndex = 0;
	for (auto& jsonInstance : j["instances"])
	{
		OpaqueInstance instance;
		math::RTransform transform;
		auto jsonInstanceRotation = jsonInstance["rotation"];
		auto jsonInstancePosition = jsonInstance["position"];
		auto jsonInstanceScale = jsonInstance["scale"];
		transform.setRotation(jsonInstanceRotation[0], jsonInstanceRotation[1], jsonInstanceRotation[2]);
		transform.setPosition(jsonInstancePosition[0], jsonInstancePosition[1], jsonInstancePosition[2]);
		transform.setScale(jsonInstanceScale[0], jsonInstanceScale[1], jsonInstanceScale[2]);
		transform.updateMatrix();
		DirectX::XMStoreFloat4x4(&instance.modelToWorld, DirectX::XMMatrixTranspose(transform.getModelMatrix()));
		instance.color = jsonInstance["color"].get<XMFLOAT4>();
		instanceIdByIndex[instanceIndex++] = utils::SolidVector<OpaqueInstance>::getInstance().insert(instance);
	}

	auto& jsonShadinGroup = j["shading-group"];
	graphics::PipelineShaders pipelineShaders;
	std::vector<OpaqueShadingGroup::PerModel> perModelArr;

	//TODO read inpulLayout from json
	std::vector<D3D11_INPUT_ELEMENT_DESC> shaderInputLayoutDesc =
	{
		// Data from the vertex buffer
		{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// Data from the instance buffer
		{ "INSTANCEMODELTOWORLD1N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEMODELTOWORLD2N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEMODELTOWORLD3N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEMODELTOWORLD4N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEOBJECTID", 0, DXGI_FORMAT_R32_UINT, 1, 80, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	const auto& exeFolderPathW = utils::FileSystem::exeFolderPathW();
	if (jsonShadinGroup.contains("vertex-shader"))
	{
		pipelineShaders.vertexShader.initialize(exeFolderPathW + utils::StringConverter::stringToWide(jsonShadinGroup["vertex-shader"].get<std::string>()), shaderInputLayoutDesc);
	}
	if (jsonShadinGroup.contains("hull-shader"))
	{
		pipelineShaders.hullShader.initialize(exeFolderPathW + utils::StringConverter::stringToWide(jsonShadinGroup["hull-shader"].get<std::string>()));
	}
	if (jsonShadinGroup.contains("domain-shader"))
	{
		pipelineShaders.domainShader.initialize(exeFolderPathW + utils::StringConverter::stringToWide(jsonShadinGroup["domain-shader"].get<std::string>()));
	}
	if (jsonShadinGroup.contains("geometry-shader"))
	{
		pipelineShaders.geometryShader.initialize(exeFolderPathW + utils::StringConverter::stringToWide(jsonShadinGroup["geometry-shader"].get<std::string>()));
	}
	if (jsonShadinGroup.contains("pixel-shader"))
	{
		pipelineShaders.pixelShader.initialize(exeFolderPathW + utils::StringConverter::stringToWide(jsonShadinGroup["pixel-shader"].get<std::string>()));
	};

	for (auto& jsonPerModel : jsonShadinGroup["per-models-array"])
	{
		OpaqueShadingGroup::PerModel perModel;

		auto optionalModel = utils::ModelManager::getInstance().getModel(jsonPerModel["model"].get<std::string>());
		if (!optionalModel.has_value())
		{
			utils::ErrorLogger::log("Failed to load model " + jsonPerModel["model"].get<std::string>());
			exit(-1);
		}
		perModel.model = optionalModel.value();

		for (auto& jsonPerMesh : jsonPerModel["per-meshes-array"])
		{
			OpaqueShadingGroup::PerMesh perMesh;
			for (auto& jsonPerMaterial : jsonPerMesh["per-materials-array"])
			{
				OpaqueShadingGroup::PerMaterial perMaterial;

				perMaterial.material.albedoMap = loadTextureFromJson(jsonPerMaterial, "albedo-texture").value_or(std::make_shared<graphics::Texture>());
				perMaterial.material.normalMap = loadTextureFromJson(jsonPerMaterial, "normal-texture").value_or(std::make_shared<graphics::Texture>());
				perMaterial.material.metallicMap = loadTextureFromJson(jsonPerMaterial, "metallic-texture").value_or(std::make_shared<graphics::Texture>());
				perMaterial.material.roughnessMap = loadTextureFromJson(jsonPerMaterial, "roughness-texture").value_or(std::make_shared<graphics::Texture>());

				for (auto& jsonInstanceID : jsonPerMaterial["instancesID"])
				{
					perMaterial.instancesID.push_back(instanceIdByIndex[jsonInstanceID]);
				}
				perMesh.perMaterialArr.push_back(perMaterial);
			}
			perModel.perMeshArr.push_back(perMesh);
		}
		perModelArr.push_back(perModel);
	}
	
	graphics::MeshSystem::getInstance().getOpaqueInstances().setPerModelArr(perModelArr);
	graphics::MeshSystem::getInstance().getOpaqueInstances().setPipelineShaders(pipelineShaders);
	graphics::MeshSystem::getInstance().getOpaqueInstances().updateInstanceBuffer();
}

void SceneLoader::loadLightDebuggerFromJson(const nlohmann::json& j)
{
	auto jDebugger = j["debugger"];
	auto jLightDebugger = jDebugger["light"];
	auto jPointLightDebugger = jLightDebugger["point-light"];
	const std::string modelPath = jPointLightDebugger["model"].get<std::string>();
	const auto optionModel = utils::ModelManager::getInstance().getModel(modelPath);
	if (!optionModel.value())
	{
		throw std::exception(("Can't load model \"" + modelPath + "\"").c_str());
	}
	const float32 radius = jPointLightDebugger["radius"].get<float32>();
	LightDebugger::getInstance().setPointLightModel(optionModel.value());
	LightDebugger::getInstance().setPointLightSphereRadius(radius);
}

void SceneLoader::loadLightSystemFromJson(const nlohmann::json& j)
{
	auto jLightSystem = j["light-system"];
	auto lightsCBS = graphics::LightSystem::getInstance().getLightCBS();
	lightsCBS.directionalLightsSize.x = 0u;
	for (auto& jDirectionalLight : jLightSystem["directional-lights"])
	{
		lightsCBS.directionalLightsCBS[lightsCBS.directionalLightsSize.x].direction = jDirectionalLight["direction"].get<XMFLOAT4>();
		lightsCBS.directionalLightsCBS[lightsCBS.directionalLightsSize.x].intensity = jDirectionalLight["intensity"].get<XMFLOAT4>();
		XMStoreFloat4(&lightsCBS.directionalLightsCBS[lightsCBS.directionalLightsSize.x].solidAngle, XMVectorReplicate(jDirectionalLight["solid-angle"].get<float32>()));
		++lightsCBS.directionalLightsSize.x;
	}
	lightsCBS.pointLightsSize.x = 0u;
	for (auto& jPointLight : jLightSystem["point-lights"])
	{
		lightsCBS.pointLightCBS[lightsCBS.pointLightsSize.x].position = jPointLight["position"].get<XMFLOAT4>();
		lightsCBS.pointLightCBS[lightsCBS.pointLightsSize.x].intensity = jPointLight["intensity"].get<XMFLOAT4>();
		XMStoreFloat4(&lightsCBS.pointLightCBS[lightsCBS.pointLightsSize.x].radius, XMVectorReplicate(jPointLight["radius"].get<float32>()));
		++lightsCBS.pointLightsSize.x;
	}
	lightsCBS.spotLightsSize.x = 0u;
	for (auto& jSpotlLight : jLightSystem["spot-lights"])
	{
		lightsCBS.spotLightsCBS[lightsCBS.spotLightsSize.x].position = jSpotlLight["position"].get<XMFLOAT4>();
		lightsCBS.spotLightsCBS[lightsCBS.spotLightsSize.x].intensity = jSpotlLight["intensity"].get<XMFLOAT4>();
		lightsCBS.spotLightsCBS[lightsCBS.spotLightsSize.x].direction = jSpotlLight["direction"].get<XMFLOAT4>();
		XMStoreFloat4(&lightsCBS.spotLightsCBS[lightsCBS.spotLightsSize.x].radius, XMVectorReplicate(jSpotlLight["radius"].get<float32>()));
		XMStoreFloat4(&lightsCBS.spotLightsCBS[lightsCBS.spotLightsSize.x].angle, XMVectorReplicate(jSpotlLight["angle"].get<float32>()));
		++lightsCBS.spotLightsSize.x;
	}
	graphics::LightSystem::getInstance().setLightCBS(lightsCBS);
}

void SceneLoader::loadPostProccesFromJson(const nlohmann::json& j)
{
	graphics::PostProcess::getInstance().setEV100(j["post-process"]["ev100"].get<float32>());
	graphics::PostProcess::getInstance().setGamma(j["post-process"]["gamma"].get<float32>());
}

std::optional<std::shared_ptr<graphics::Texture>> SceneLoader::loadTextureFromJson(const nlohmann::json& j, const std::string& textureType)
{
	if (!j.contains(textureType))
		return std::nullopt;
	std::string texturePath = j[textureType].get<std::string>();
	std::wstring texturePathW = utils::StringConverter::stringToWide(texturePath);
	auto optionalTexture = utils::TextureManager::getInstance().getTexture(texturePathW);
	if (!optionalTexture.has_value())
		throw std::exception(("Can't load texture \"" + texturePath + "\"").c_str());
	return optionalTexture.value();
}