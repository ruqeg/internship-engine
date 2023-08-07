#include "LightDebugger.h"

void LightDebugger::initialize(const std::shared_ptr<graphics::Model>& pointlightModel)
{
	graphics::PipelineShaders pipelineShaders;
	graphics::MeshSystem::OpaqueShadingGroup::PerModel perModel;

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
		{ "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	pipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + L"light_debugger_vertex.cso", shaderInputLayoutDesc);
	pipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + L"light_debugger_pixel.cso");

	perModel.model = pointlightModel;

	perModel.perMeshArr.resize(1);
	perModel.perMeshArr[0].perMaterialArr.resize(1);
	


	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().setPipelineShaders(pipelineShaders);
	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().setPerModelArr({ perModel });
}

void LightDebugger::instertLightSystemInstances()
{
	instanceIdLightIndexMap.clear();

	graphics::LightCBS lightCBS = graphics::LightSystem::getInstance().getLightCBS();
	int32 numInstances = lightCBS.pointLightsSize.x;

	auto& perModelArr = graphics::MeshSystem::getInstance().getDebugOpaqueInstances().getPerModelArr();
	auto& instancesID = perModelArr[0].perMeshArr[0].perMaterialArr[0].instancesID;
	instancesID.resize(numInstances);

	for (uint32 i = 0; i < numInstances; ++i)
	{
		SolidVectorOpaqueInstance::ID instanceID = SolidVectorOpaqueInstance::getInstance().insert({});
		instanceIdLightIndexMap[instanceID] = i;
		instancesID[i] = instanceID;
	}

	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().setPerModelArr(perModelArr);
}

void LightDebugger::update()
{
	graphics::LightCBS lightCBS = graphics::LightSystem::getInstance().getLightCBS();
	for (auto& [instanceID, lightIndex] : instanceIdLightIndexMap)
	{
		SolidVectorOpaqueInstance::getInstance()[instanceID] = instanceFromPointlightCBS(lightCBS.pointLightCBS[lightIndex]);
	}
}

const graphics::MeshSystem::OpaqueInstanceGPU& LightDebugger::instanceFromPointlightCBS(const graphics::PointLightsCBS& pointLightCBS)
{
	graphics::MeshSystem::OpaqueInstanceGPU instance;

	const float32 pointLightSphereDiameter = 2.f * pointLightSphereRadius * pointLightCBS.radius.x;

	const XMMATRIX translateMat = XMMatrixTranslation(pointLightCBS.position.x, pointLightCBS.position.y, pointLightCBS.position.z);
	const XMMATRIX scaleMat = XMMatrixScaling(pointLightSphereDiameter, pointLightSphereDiameter, pointLightSphereDiameter);

	XMStoreFloat4x4(&instance.modelToWorld, XMMatrixTranspose(XMMatrixMultiply(scaleMat, translateMat)));
	instance.color = pointLightCBS.intensity;

	return instance;
}

bool LightDebugger::isPointlightInstanceID(SolidVectorOpaqueInstance::ID instanceID)
{
	return instanceIdLightIndexMap.find(instanceID) != instanceIdLightIndexMap.end();
}

void LightDebugger::changePointlight(SolidVectorOpaqueInstance::ID instanceID)
{
	auto& instance = SolidVectorOpaqueInstance::getInstance()[instanceID];
	auto& modelToWorld = instance.modelToWorld;

	graphics::LightCBS lightCBS = graphics::LightSystem::getInstance().getLightCBS();
	auto& pointlight = lightCBS.pointLightCBS[instanceIdLightIndexMap[instanceID]];
	DirectX::XMVECTOR position = DirectX::XMVectorSet(modelToWorld._14, modelToWorld._24, modelToWorld._34, modelToWorld._44);
	DirectX::XMStoreFloat4(&pointlight.position, position);

	graphics::LightSystem::getInstance().setLightCBS(lightCBS);
}

void LightDebugger::setPointLightSphereRadius(const float32 pointLightSphereRadius)
{
	this->pointLightSphereRadius = pointLightSphereRadius;
}

float32 LightDebugger::getPointLightSphereRadius() const
{
	return pointLightSphereRadius;
}

void LightDebugger::setPointLightModel(const std::shared_ptr<graphics::Model>& model)
{
	auto& perModelArr = graphics::MeshSystem::getInstance().getDebugOpaqueInstances().getPerModelArr();
	perModelArr[0].model = model;
	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().setPerModelArr(perModelArr);
	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().updateInstanceBuffer();
}

LightDebugger& LightDebugger::getInstance()
{
	static LightDebugger instance;
	return instance;
}