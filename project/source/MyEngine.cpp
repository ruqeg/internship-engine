#include "MyEngine.h"


void MyEngine::d3dinit()
{
	d3d::D3D::getInstance().init([](const IDXGIAdapter* pIDXGIAdapter, const DXGI_ADAPTER_DESC& description) -> uint32 {
		//Example
		//Choose best videocard
		if (std::wstring(description.Description).find(L"NVIDIA GeForce RTX 3050 Laptop GPU") != std::string::npos)
		{
			return 100u;
		}
		else
		{
			return 1u;
		}
	});
	graphics::MeshSystem::getInstance().initialize();
	utils::ModelManager::getInstance().initialize();
	IntersectionSystem::getInstance().initialize();
	graphics::LightSystem::getInstance().initialize();
	graphics::PostProcess::getInstance().initialize();
}

void MyEngine::d3ddeinit()
{
	d3d::D3D::getInstance().deinit();
}

void MyEngine::initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int32 width, int32 height)
{
	renderWindow.initialize(this, hInstance, windowTitle, windowClass, width, height);
	
	initializeSwapchain(renderWindow.GetHWND());
	initializeBackBuffer();
	initializeDepthBuffer();
	initializeRenderTargetView();
	initializeViewport(width, height);


	UI::getInstance().initialize(renderWindow.GetHWND());
	initializeConstantBuffers();


	fullscreenTriangle.initialize();
	loadFromJson("../resources/scenes/scene_main.json");
	fullscreenTriangle.update(3, camera);

	initializeDebugShaders();
	initializeSamplerState();
	initializeShadowTexturesAndSRVs();

	initalizeDissolutionShadingGroup();

	cameraController.setMovingSpeed(1.0f);
	cameraController.setRotatingSpeed(0.15f);

	UI::getInstance().setViewTextureFilter(0);

	flashlightRextureMask = utils::TextureManager::getInstance().getTexture(L"../resources/textures/flashlight_mask.dds").value();
	fireNoiseTexture = utils::TextureManager::getInstance().getTexture(L"../resources/textures/Noise_16.dds").value();

	initializeSceneRTV();
	initializeHDRSceneRTV();
	initializeSceneBlendState();

	initializeSceneDepthTextureSRVCopy();

	initializeParticelSystem();

	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().updateInstanceBuffer();
	initializeIntersectionSystem();
	initializeSceneDepthStencilState();
	initalizeIncinerationShadingGroup();

	graphics::DeferredShading::getInstance().initialize(backbufferDesc.Width, backbufferDesc.Height);
	initializeNormalWithoutIncinerationTextureAndSRV();

	graphics::FullscreenPass::getInstance().initialize();

	initializePostProcess();
	int32 clientW, clientH;
	MyUtils::getClientSize(renderWindow.GetHWND(), clientW, clientH);
	graphics::DecalSystem::getInstance().initialize(clientW, clientH);

	graphics::MeshSystem::getInstance().updateModelInstancesIDs();
	graphics::MeshSystem::getInstance().getOpaqueInstances().updateInstanceBuffer();
}

bool MyEngine::processMessages()
{
	return renderWindow.processMessages();
}

void MyEngine::update(float64 elapsed)
{
	bool cameraChanged = 0;

	while (!keyboard.charBufferIsEmpty())
	{
		uint8 ch = keyboard.readChar();
	}
	while (!keyboard.keyBufferIsEmpty())
	{
		window::KeyboardEvent kbe = keyboard.readKey();
		uint8 keycode = kbe.getKeyCode();

		if (kbe.isPress() && keycode == 'N')
			spawnModelInFrontOfCamera(modelSpawnDistance);
		if (kbe.isPress() && keycode == 'G')
			spawnDecalInstance();
		if (kbe.isPress() && keycode == VK_DELETE)
			deleteOpaqueInstanceHandler();
	}
	while (!windowEventsBufferIsEmpty())
	{
		if (readWindowEvent().isResized())
			resizeEvent();
	}
	while (!mouse.eventBufferIsEmpty())
	{
		window::MouseEvent me = mouse.readEvent();

		cameraChanged |= cameraController.updateRotating(mouse, me, oldMousePoint, elapsed, camera);

		if (keyboard.keyIsPressed('X'))
			updateDragging(me);

		if (me.getType() == window::MouseEvent::EventType::Move)
			oldMousePoint = me.getPos();
	}
	while (!UI::getInstance().eventsBufferIsEmpty())
	{
		UI::Event e = UI::getInstance().readEvent();
		if (e.getType() == UI::Event::LoadScene)
			reinitializeFromJson(UI::getInstance().getSceneFilePath());
		if (e.getType() == UI::Event::LightDebuggerUpdated)
			initializeIntersectionSystem();
		if (e.getType() == UI::Event::SpawnEmmiter)
			spawnEmmiterFromUI();
	}
	
	cameraChanged |= cameraController.updateMoving(keyboard, elapsed, camera);

	if (cameraChanged)
	{
		camera.updateViewMatrix();
		camera.updateMainMatrix();

		fullscreenTriangle.update(3, camera);

		if (UI::getInstance().getDetachFlashlightFromCamera())
			updateCamerFlashlight();
	}
	
	if (graphics::ParticleSystem::getInstance().updateEmmiters(elapsed, camera) == graphics::ParticleSystem::PARTICLES_UPDATED)
	{
		graphics::ParticleSystem::getInstance().updateInstanceBuffer();
	}

	updateDissolutionShadingGroupInstances(elapsed);
	updateIncinerationShadingGroupInstances(elapsed);
	
	updateOutdatedDissolutionInstances();
	updateOutdatedIncinerationInstances();

	initializeIntersectionSystem();

	graphics::MeshSystem::getInstance().getIncinerationShadingGroup().updateInstanceBuffer();
	graphics::MeshSystem::getInstance().getDissolutionShadingGroup().updateInstanceBuffer();
	graphics::MeshSystem::getInstance().getOpaqueInstances().updateInstanceBuffer();
}

void MyEngine::renderFrame(float64 elapsed)
{
	const std::array<float, 4> clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	prerenderSceneRTV(clearColor);

	bindConstantBuffers();

	bindSamplerStates();

	d3d::devcon->PSSetShaderResources(11, 1, flashlightRextureMask->shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(12, 1, fireNoiseTexture->shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(20, 1, diffuseIrradianceCubemap->shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(21, 1, specularReflectionCubemap->shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(22, 1, specularReflectanceTexture2D->shaderResourceView.GetAddressOf());

	d3d::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	renderShadowMaps();

	shadowMapsArrayCBs.applyChanges(ShadowMapArrayConstantBuffer::shaderRegister, graphics::MapTarget::PS);

	setClientSizeViewport();

	d3d::devcon->PSSetShaderResources(23, shadowsMaps2DSRVs.size(), shadowsMaps2DSRVs.front().GetAddressOf());

	d3d::devcon->PSSetShaderResources(35, shadowsMaps3DSRVs.size(), shadowsMaps3DSRVs.front().GetAddressOf());

	if (UI::getInstance().getViewSceneShaders())
	{
		graphics::ShadingGroupRenderBindTarget bindTarget = graphics::ShadingGroupRenderBindTarget(
			meshStartSlot,
			meshMapTarget,
			materialStartSlot,
			materialMapTarget,
			albedoStartSlot,
			metallicStartSlot,
			roughnessStartSlot,
			normalMapStartSlot);

		graphics::DeferredShading::getInstance().prerender({ 0, 0, 0, 0 });

		d3d::devcon->OMSetDepthStencilState(depthStencilStateFill.Get(), 1);
		graphics::DeferredShading::getInstance().renderOpaque(graphics::MeshSystem::getInstance().getOpaqueInstances(), bindTarget);
		graphics::DeferredShading::getInstance().renderOpaque(graphics::MeshSystem::getInstance().getDissolutionShadingGroup(), bindTarget);

		d3d::devcon->OMSetDepthStencilState(depthStencilStateFill.Get(), 0);
		d3d::devcon->CopyResource(depthWithoutIncinerationTexture.Get(), graphics::DeferredShading::getInstance().getDepthStencilBuffer().Get());
		d3d::devcon->CopyResource(normalWithoutIncinerationTexture.Get(), graphics::DeferredShading::getInstance().getGBuffer().normallBuffer.texture2d.Get());

		d3d::devcon->OMSetDepthStencilState(depthStencilStateFill.Get(), 2);
		graphics::DeferredShading::getInstance().renderEmission(graphics::MeshSystem::getInstance().getDebugOpaqueInstances(), bindTarget);

		auto& ringBuffer = graphics::ParticleSystem::getInstance().getRingBuffer();

		d3d::devcon->OMSetDepthStencilState(depthStencilStateFill.Get(), 1);
		graphics::DeferredShading::getInstance().renderOpaque(
			graphics::MeshSystem::getInstance().getIncinerationShadingGroup(),
			bindTarget,
			5u,
			{ ringBuffer.getParticleBufferUAV().Get() , ringBuffer.getRangeBufferUAV().Get() });

		auto& gbuffer = graphics::DeferredShading::getInstance().getGBuffer();
		graphics::DecalSystem::getInstance().render(
			gbuffer.albedoBuffer.renderTargetView,
			gbuffer.normallBuffer.renderTargetView,
			gbuffer.roughnessMetallnessBuffer.renderTargetView,
			gbuffer.emissionBuffer.renderTargetView,
			gbuffer.objectIDBuffer.shaderResourceView,
			graphics::DeferredShading::getInstance().getDepthStencilBuffer().Get(),
			gbuffer.normallBuffer.texture2d,
			depthStencilView);

		d3d::devcon->CopyResource(depthStencilBuffer.Get(), graphics::DeferredShading::getInstance().getDepthStencilBuffer().Get());

		d3d::devcon->OMSetRenderTargets(1, sceneRTV.GetAddressOf(), depthStencilView.Get());
		graphics::DeferredShading::getInstance().bind(graphics::FullscreenPass::getInstance().getDeferredBindTarget());

		d3d::devcon->CSSetShaderResources(1u, 1u, normalWithoutIncinerationSRV.GetAddressOf());
		d3d::devcon->CSSetShaderResources(2u, 1u, depthWithoutIncinerationSRV.GetAddressOf());
		graphics::ParticleSystem::getInstance().updateGPUParticles(elapsed);

		d3d::devcon->PSSetShaderResources(7u, 1u, graphics::ParticleSystem::getInstance().getRingBuffer().getParticleBufferSRV().GetAddressOf());
		d3d::devcon->PSSetShaderResources(8u, 1u, graphics::ParticleSystem::getInstance().getRingBuffer().getRangeBufferSRV().GetAddressOf());

		d3d::devcon->OMSetDepthStencilState(depthStencilStateEqual.Get(), 1);
		graphics::FullscreenPass::getInstance().renderPBR();

		d3d::devcon->OMSetDepthStencilState(depthStencilStateEqual.Get(), 2);
		graphics::FullscreenPass::getInstance().renderEmission();
	}

	d3d::devcon->OMSetDepthStencilState(depthStencilStateFill.Get(), 3);
	
	if (UI::getInstance().getViewNormalShaders() || UI::getInstance().getViewWireframeShaders())
	{
		renderDebug();
	}
	
	ID3D11ShaderResourceView* nu[] = {nullptr};
	d3d::devcon->PSSetShaderResources(23, 1, nu);
	
	fullscreenTriangle.render(10);
	
	d3d::devcon->CopyResource(depthBufferCopy.Get(), depthStencilBuffer.Get());
	d3d::devcon->PSSetShaderResources(13, 1, depthCopyStencilViewSRV.GetAddressOf());

	d3d::devcon->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);
	graphics::ParticleSystem::getInstance().render(8u, 14u, 15u, 16u);
	d3d::devcon->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	
	graphics::PostProcess::getInstance().applyHDR(sceneSRV, hdrSceneRTV);
	graphics::PostProcess::getInstance().applyFXAA(hdrSceneSRV, renderTargetView);
	
	UI::getInstance().render();

	postRender();
}

void MyEngine::renderDebug()
{
	if (UI::getInstance().getViewNormalShaders())
	{
		graphics::MeshSystem::getInstance().render(
			graphics::ShadingGroupRenderBindTarget(
				meshStartSlot,
				meshMapTarget,
				materialStartSlot,
				materialMapTarget,
				albedoStartSlot,
				metallicStartSlot,
				roughnessStartSlot,
				normalMapStartSlot
			),
			depthStencilStateFill,
			normalDebugShaders);
	}
	if (UI::getInstance().getViewWireframeShaders())
	{
		graphics::MeshSystem::getInstance().render(
			graphics::ShadingGroupRenderBindTarget(
				meshStartSlot,
				meshMapTarget,
				materialStartSlot,
				materialMapTarget,
				albedoStartSlot,
				metallicStartSlot,
				roughnessStartSlot,
				normalMapStartSlot
			),
			depthStencilStateFill,
			wireframeDebugShaders);
	}
}

void MyEngine::renderShadowMaps()
{
	renderSpotlightShadowMaps();
	renderSunlightShadowMaps();
	renderPointlightShadowMaps();
}

void MyEngine::renderSpotlightShadowMaps()
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = static_cast<float32>(spotlightSize);
	viewport.Height = static_cast<float32>(spotlightSize);
	d3d::devcon->RSSetViewports(1, &viewport);

	auto& lightCBS = graphics::LightSystem::getInstance().getLightCBS();

	const int indexOffset = 4;
	for (auto i = 0; i < lightCBS.spotLightsSize.x; ++i)
	{
		updateSpotlightShadowMapCB(lightCBS.spotLightsCBS[i], spotlightSize);
		shadowMap2DCB.applyChanges(graphics::ShadowMap2DConstantBuffer::shaderRegister, graphics::MapTarget::VS);

		shadowMapsArrayCBs.data.shadowMaps2DCBs[indexOffset + i] = shadowMap2DCB.data;
		createShadowDepthStencilView(shadowMapTextures2D[indexOffset + i]);
		d3d::devcon->OMSetRenderTargets(0, 0, shadowDepthStencilView.Get());
		d3d::devcon->ClearDepthStencilView(shadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

		graphics::MeshSystem::getInstance().renderDepth2D(graphics::ShadingGroupRenderBindTarget(2, graphics::MapTarget::VS, 3, graphics::MapTarget::PS, 0, 1, 2, 3));
	}
}

void MyEngine::renderSunlightShadowMaps()
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = static_cast<float32>(directionallightSize);
	viewport.Height = static_cast<float32>(directionallightSize);
	d3d::devcon->RSSetViewports(1, &viewport);

	const auto& lightCBS = graphics::LightSystem::getInstance().getLightCBS();

	for (auto i = 0; i < lightCBS.directionalLightsSize.x; ++i)
	{
		updateDirectionallightShadowMapCB(lightCBS.directionalLightsCBS[i], directionallightSize);
		shadowMap2DCB.applyChanges(graphics::ShadowMap2DConstantBuffer::shaderRegister, graphics::MapTarget::VS);

		shadowMapsArrayCBs.data.shadowMaps2DCBs[i] = shadowMap2DCB.data;
		createShadowDepthStencilView(shadowMapTextures2D[i]);
		d3d::devcon->OMSetRenderTargets(0, 0, shadowDepthStencilView.Get());
		d3d::devcon->ClearDepthStencilView(shadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

		graphics::MeshSystem::getInstance().renderDepth2D(graphics::ShadingGroupRenderBindTarget(2, graphics::MapTarget::VS, 3, graphics::MapTarget::PS, 0, 1, 2, 3));
	}
}

void MyEngine::renderPointlightShadowMaps()
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = static_cast<float32>(pointlightSize);
	viewport.Height = static_cast<float32>(pointlightSize);
	d3d::devcon->RSSetViewports(1, &viewport);

	auto& lightCBS = graphics::LightSystem::getInstance().getLightCBS();

	for (auto i = 0; i < lightCBS.pointLightsSize.x; ++i)
	{
		updatePointlightShadowMapCB(lightCBS.pointLightCBS[i], spotlightSize);
		shadowMap3DCB.applyChanges(graphics::ShadowMap2DConstantBuffer::shaderRegister, graphics::MapTarget::GS);

		shadowMapsArrayCBs.data.shadowMaps3DCBs[i] = shadowMap3DCB.data;
		createShadow3DDepthStencilView(shadowMapTextures3D[i]);
		d3d::devcon->OMSetRenderTargets(0, 0, shadowDepthStencilView.Get());
		d3d::devcon->ClearDepthStencilView(shadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

		graphics::MeshSystem::getInstance().renderDepthCubemap(graphics::ShadingGroupRenderBindTarget(2, graphics::MapTarget::VS, 3, graphics::MapTarget::PS, 0, 1, 2, 3));
	}
}

void MyEngine::initializeConstantBuffers()
{
	//TDOO
	HRESULT hr = plasmaConstantBuffer.initialize();
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create plasmaConstantBuffer.");
		exit(-1);
	}

	hr = cameraConstantBuffer.initialize();
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create cameraConstantBuffer.");
		exit(-1);
	}

	hr = textureViewConstantBuffer.initialize();
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create textureViewConstantBuffer.");
		exit(-1);
	}

	hr = IBLCB.initialize();
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create IBLCB.");
		exit(-1);
	}

	hr = shadowMap2DCB.initialize();
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create shadow map 2D Constant Buffer.");
		exit(-1);
	}

	hr = shadowMapsArrayCBs.initialize(); 
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create shadow map shadowMaps2DArrayCBs.");
		exit(-1);
	}

	hr = shadowMap3DCB.initialize();
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create shadow map shadowMaps2DArrayCBs.");
		exit(-1);
	}
}

void MyEngine::initializeIntersectionSystem()
{
	auto& meshSystemInstance = graphics::MeshSystem::getInstance();
	std::array<graphics::MeshSystem::OpaqueShadingGroup*, 2> opaqueInstancesArr = { &meshSystemInstance.getOpaqueInstances(), &meshSystemInstance.getDebugOpaqueInstances()};
	std::vector<IntersectionSystem::PerModel> iPerModelsArr;
	for (auto& opauqeInstance : opaqueInstancesArr)
	{
		for (auto& perModel : opauqeInstance->getPerModelArr())
		{
			IntersectionSystem::PerModel iPerModel;
			iPerModel.perMaterials.resize(perModel.perMeshArr.front().perMaterialArr.size());
			iPerModel.model = perModel.model;
			for (auto& perMesh : perModel.perMeshArr)
			{
				for (uint32 i = 0; i < perMesh.perMaterialArr.size(); ++i)
				{
					const auto perMaterial = perMesh.perMaterialArr[i];

					if (perMaterial.instancesID.empty())
						break;

					iPerModel.perMaterials[i].instanceID = perMaterial.instancesID;
				}
			}
			iPerModelsArr.push_back(iPerModel);
		}
	}
	IntersectionSystem::getInstance().update(iPerModelsArr);
	currentIntersectionSystemRec.model = std::make_shared<graphics::Model>();
}

void MyEngine::reinitializeRenderTargetView(int32 width, int32 height)
{
	renderTargetView.ReleaseAndGetAddressOf();
	initializeBackBuffer();
	initializeDepthBuffer();
	initializeRenderTargetView();
	initializeViewport(width, height);
}

void MyEngine::initializeSwapchain(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC1 scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC1));

	scd.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
	scd.BufferCount = 2;
	scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = 0;
	scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Scaling = DXGI_SCALING_NONE;
	scd.Stereo = false;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr = d3d::factory->CreateSwapChainForHwnd(
		d3d::device,
		hwnd,
		&scd,
		NULL,
		NULL,
		swapchain.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create swapchain.");
		exit(0);
	}
}

void MyEngine::initializeBackBuffer()
{
	if (backbuffer != nullptr)
	{
		backbuffer.ReleaseAndGetAddressOf();
		swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	}

	HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backbuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to get buffer.");
		exit(0);
	}

	ID3D11Texture2D* pTextureInterface = 0;
	backbuffer->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	pTextureInterface->GetDesc(&backbufferDesc);
	pTextureInterface->Release();
}

void MyEngine::initializeDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = backbufferDesc.Width;
	depthStencilDesc.Height = backbufferDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HRESULT hr = d3d::device->CreateTexture2D(&depthStencilDesc, NULL, depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create depth stencil buffer");
		exit(-1);
	}

	hr = d3d::device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create depth stencil view");
		exit(-1);
	}
}

void MyEngine::initializeRenderTargetView()
{
	HRESULT hr = d3d::device->CreateRenderTargetView(backbuffer.Get(), NULL, renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create render target view.");
		exit(0);
	}
}

void MyEngine::initializeViewport(int32 width, int32 height)
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;
	d3d::devcon->RSSetViewports(1, &viewport);
}

void MyEngine::preRender()
{

}

void MyEngine::postRender()
{
	swapchain->Present(0, NULL);
}

void MyEngine::initializeDebugShaders()
{
	//TODO read from json
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
	};
	const auto& exeFolderPathW = utils::FileSystem::exeFolderPathW();
	{
		normalDebugShaders.vertexShader.initialize(exeFolderPathW + L"normal_visualization_vertex.cso", shaderInputLayoutDesc);
		normalDebugShaders.geometryShader.initialize(exeFolderPathW + L"normal_visualization_geometry.cso");
		normalDebugShaders.pixelShader.initialize(exeFolderPathW + L"normal_visualization_pixel.cso");
	}
	{
		wireframeDebugShaders.vertexShader.initialize(exeFolderPathW + L"wireframe_visualization_vertex.cso", shaderInputLayoutDesc);
		wireframeDebugShaders.geometryShader.initialize(exeFolderPathW + L"wireframe_visualization_geometry.cso");
		wireframeDebugShaders.pixelShader.initialize(exeFolderPathW + L"wireframe_visualization_pixel.cso");
	}
}

void MyEngine::initializeSamplerState()
{
	D3D11_SAMPLER_DESC generalDesc;
	ZeroMemory(&generalDesc, sizeof(D3D11_SAMPLER_DESC));
	generalDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	generalDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	generalDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	generalDesc.ComparisonFunc = D3D11_COMPARISON_NEVER; 
	generalDesc.MinLOD = 0;
	generalDesc.MaxLOD = D3D11_FLOAT32_MAX;

	generalDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
	d3d::device->CreateSamplerState(&generalDesc, samplerStates[0].GetAddressOf());

	generalDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3d::device->CreateSamplerState(&generalDesc, samplerStates[1].GetAddressOf());

	generalDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
	d3d::device->CreateSamplerState(&generalDesc, samplerStates[2].GetAddressOf());

	generalDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	generalDesc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	d3d::device->CreateSamplerState(&generalDesc, samplerStateDepthAnisotropic.GetAddressOf());
}

void MyEngine::initializeRasterizer()
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	//rasterizerDesc.DepthBias = -2;
	//rasterizerDesc.DepthBiasClamp = 0;
	//rasterizerDesc.SlopeScaledDepthBias = -2;
	
	HRESULT hr = d3d::device->CreateRasterizerState(&rasterizerDesc, rasterizerState.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create rasterizer state");
		exit(-1);
	}
}

void MyEngine::initializeSceneRTV()
{
	int32 width, height;
	MyUtils::getClientSize(renderWindow.GetHWND(), width, height);

	CD3D11_TEXTURE2D_DESC sceneDesc(
		DXGI_FORMAT_R16G16B16A16_FLOAT, width, height,
		1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	d3d::device->CreateTexture2D(&sceneDesc, nullptr, sceneTexture.ReleaseAndGetAddressOf());
	d3d::device->CreateShaderResourceView(sceneTexture.Get(), nullptr, sceneSRV.ReleaseAndGetAddressOf());
	d3d::device->CreateRenderTargetView(sceneTexture.Get(), nullptr, sceneRTV.ReleaseAndGetAddressOf());
	
}

void MyEngine::initializeHDRSceneRTV()
{
	int32 width, height;
	MyUtils::getClientSize(renderWindow.GetHWND(), width, height);

	CD3D11_TEXTURE2D_DESC sceneDesc(
		DXGI_FORMAT_R8G8B8A8_UNORM, width, height,
		1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	d3d::device->CreateTexture2D(&sceneDesc, nullptr, hdrSceneTexture.ReleaseAndGetAddressOf());
	d3d::device->CreateShaderResourceView(hdrSceneTexture.Get(), nullptr, hdrSceneSRV.ReleaseAndGetAddressOf());
	d3d::device->CreateRenderTargetView(hdrSceneTexture.Get(), nullptr, hdrSceneRTV.ReleaseAndGetAddressOf());
}

void MyEngine::initializeShadowTexturesAndSRVs()
{
	shadowMapTextures2D.resize(shadowMapTexturesSpotlightNum + shadowMapTexturesDirectionalLightNum);
	shadowsMaps2DSRVs.resize(shadowMapTexturesSpotlightNum + shadowMapTexturesDirectionalLightNum);
	shadowMapTextures3D.resize(shadowMapTexturesPointlightNum);
	shadowsMaps3DSRVs.resize(shadowMapTexturesPointlightNum);

	D3D11_TEXTURE2D_DESC depthTex2Ddesc{};
	depthTex2Ddesc.MipLevels = 1;
	depthTex2Ddesc.ArraySize = 1;
	depthTex2Ddesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthTex2Ddesc.SampleDesc.Count = 1;
	depthTex2Ddesc.SampleDesc.Quality = 0;
	depthTex2Ddesc.Usage = D3D11_USAGE_DEFAULT;
	depthTex2Ddesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTex2Ddesc.CPUAccessFlags = 0;
	depthTex2Ddesc.MiscFlags = 0;

	depthTex2Ddesc.Width = directionallightSize;
	depthTex2Ddesc.Height = directionallightSize;

	for (uint32 i = 0; i < shadowMapTexturesDirectionalLightNum; ++i)
		HRESULT hr = d3d::device->CreateTexture2D(&depthTex2Ddesc, nullptr, shadowMapTextures2D[i].ReleaseAndGetAddressOf());

	depthTex2Ddesc.Width = spotlightSize;
	depthTex2Ddesc.Height = spotlightSize;

	for (uint32 i = 0; i < shadowMapTexturesSpotlightNum; ++i)
		HRESULT hr = d3d::device->CreateTexture2D(&depthTex2Ddesc, nullptr, shadowMapTextures2D[shadowMapTexturesDirectionalLightNum + i].ReleaseAndGetAddressOf());

	D3D11_TEXTURE2D_DESC depthTexCubeDesc{};
	depthTexCubeDesc.MipLevels = 1;
	depthTexCubeDesc.ArraySize = 6;
	depthTexCubeDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthTexCubeDesc.SampleDesc.Count = 1;
	depthTexCubeDesc.SampleDesc.Quality = 0;
	depthTexCubeDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexCubeDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexCubeDesc.CPUAccessFlags = 0;
	depthTexCubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	depthTexCubeDesc.Width = pointlightSize;
	depthTexCubeDesc.Height = pointlightSize;

	for (uint32 i = 0; i < shadowMapTexturesPointlightNum; ++i)
		HRESULT hr = d3d::device->CreateTexture2D(&depthTexCubeDesc, nullptr, shadowMapTextures3D[i].ReleaseAndGetAddressOf());

	
	D3D11_SHADER_RESOURCE_VIEW_DESC srv2Ddesc {};
	srv2Ddesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srv2Ddesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv2Ddesc.Texture2D.MipLevels = 1;

	for (uint32 i = 0; i < shadowMapTexturesDirectionalLightNum + shadowMapTexturesSpotlightNum; ++i)
		d3d::device->CreateShaderResourceView(shadowMapTextures2D[i].Get(), &srv2Ddesc, shadowsMaps2DSRVs[i].ReleaseAndGetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvCubeDesc {};
	srvCubeDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvCubeDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvCubeDesc.TextureCube.MipLevels = 1;
	for (uint32 i = 0; i < shadowMapTexturesPointlightNum; ++i)
		d3d::device->CreateShaderResourceView(shadowMapTextures3D[i].Get(), &srvCubeDesc, shadowsMaps3DSRVs[i].ReleaseAndGetAddressOf());
}

void MyEngine::initalizeDissolutionShadingGroup()
{
	//TODO read inpulLayout from json
	const std::vector<D3D11_INPUT_ELEMENT_DESC> dissoultionShaderInputLayoutDesc =
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
		{ "INSTANCETIME", 0, DXGI_FORMAT_R32_FLOAT, 1, 80, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	const auto& exeFolderPathW = utils::FileSystem::exeFolderPathW();

	graphics::PipelineShaders pipelineShaders;
	pipelineShaders.vertexShader.initialize(exeFolderPathW + dissolutionShadingGroupVertexShaderFilename, dissoultionShaderInputLayoutDesc);
	pipelineShaders.pixelShader.initialize(exeFolderPathW + dissolutionShadingGroupPixelShaderFilename);
	graphics::MeshSystem::getInstance().getDissolutionShadingGroup().setPipelineShaders(pipelineShaders);
}

// !TODO upgrade code
void MyEngine::initalizeIncinerationShadingGroup()
{
	//TODO read inpulLayout from json
	const std::vector<D3D11_INPUT_ELEMENT_DESC> dissoultionShaderInputLayoutDesc =
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
		{ "RAYINTERSECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 80, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCETIME", 0, DXGI_FORMAT_R32_FLOAT, 1, 92, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEPREVBOUNDINGSHPERERADIUS", 0, DXGI_FORMAT_R32_FLOAT, 1, 96, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCECURRENTBOUNDINGSHPERERADIUS", 0, DXGI_FORMAT_R32_FLOAT, 1, 100, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEMAXBOUNDINGSHPERERADIUS", 0, DXGI_FORMAT_R32_FLOAT, 1, 104, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	const auto& exeFolderPathW = utils::FileSystem::exeFolderPathW();

	graphics::PipelineShaders pipelineShaders;
	pipelineShaders.vertexShader.initialize(exeFolderPathW + incierationShadingGroupVertexShaderFilename, dissoultionShaderInputLayoutDesc);
	pipelineShaders.pixelShader.initialize(exeFolderPathW + incierationShadingGroupPixelShaderFilename);
	graphics::MeshSystem::getInstance().getIncinerationShadingGroup().setPipelineShaders(pipelineShaders);
}

void MyEngine::initializeSceneBlendState()
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	d3d::device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
}

void MyEngine::initializeSceneDepthTextureSRVCopy()
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBuffer->GetDesc(&depthStencilBufferDesc);

	D3D11_TEXTURE2D_DESC sceneDepthTexDesc = depthStencilBufferDesc;
	sceneDepthTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	sceneDepthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = d3d::device->CreateTexture2D(&sceneDepthTexDesc, nullptr, depthBufferCopy.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Error to create sceneDepthTexture!");
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = d3d::device->CreateShaderResourceView(depthBufferCopy.Get(), &srvDesc, depthCopyStencilViewSRV.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Error to create sceneDepthStencilViewSRV!");
		return;
	}

	hr = d3d::device->CreateTexture2D(&sceneDepthTexDesc, nullptr, depthWithoutIncinerationTexture.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Error to create sceneDepthTexture!");
		return;
	}

	hr = d3d::device->CreateShaderResourceView(depthWithoutIncinerationTexture.Get(), &srvDesc, depthWithoutIncinerationSRV.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Error to create sceneDepthStencilViewSRV!");
		return;
	}
}

void MyEngine::initializeParticelSystem()
{
	graphics::ParticleSystem::getInstance().initialize(MAX_GPU_PARTICLES_NUM);
	uint32 positionID = utils::SolidVector<graphics::EmmiterPosition>::getInstance().insert(DirectX::XMVectorSet(0, 3.f, 0.f, 0));
	graphics::ParticleSystem::getInstance().addSmokeEmitter(
		positionID,
		DirectX::XMVectorSet(1, 1, 1, 0.f),
		DirectX::XMVectorSet(2.f, 4.f, 0.f, 0.f),
		DirectX::XMVectorSet(0.f, 1.5f, 0.f, 0.f),
		15,
		1.f,
		21);
	EmmiterDebugger::getInstance().addDebugger(
		utils::ModelManager::getInstance().getModel(emmiterDebbugingModelPath).value(),
		DirectX::XMVectorSet(emmiterDebuggingColor[0], emmiterDebuggingColor[1], emmiterDebuggingColor[2], emmiterDebuggingColor[3]),
		emmiterDebuggingModelScale,
		positionID);
	graphics::ParticleSystem::getInstance().updateInstanceBuffer();

	graphics::RangeBufferCBS rangeBuffer{};
	graphics::ParticleSystem::getInstance().getRingBuffer().updateRangeBuffer(rangeBuffer);
}

void MyEngine::initializeSceneDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	// Configure the depth test
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	
	// Configure the stencil test
	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateDepthStencilState(&depthStencilDesc, depthStencilStateFill.GetAddressOf()),
		L"Failed to create depth stencil state");
	
	// Configure the depth test
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

	// Configure the stencil test
	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateDepthStencilState(&depthStencilDesc, depthStencilStateEqual.GetAddressOf()),
		L"Failed to create depth stencil state");
}

void MyEngine::initializePostProcess()
{
	int32 width, height;
	MyUtils::getClientSize(renderWindow.GetHWND(), width, height);
	graphics::PostProcess::getInstance().setQualityEdgeThreshold(0.063f);
	graphics::PostProcess::getInstance().setQualityEdgeThresholdMin(0.0312f);
	graphics::PostProcess::getInstance().setQualitySubpix(0.75f);
	graphics::PostProcess::getInstance().setFXAAImageSize(width, height);
}

void MyEngine::initializeNormalWithoutIncinerationTextureAndSRV()
{
	D3D11_TEXTURE2D_DESC textureDesc;
	graphics::DeferredShading::getInstance().getGBuffer().normallBuffer.texture2d->GetDesc(&textureDesc);

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateTexture2D(&textureDesc, nullptr, normalWithoutIncinerationTexture.ReleaseAndGetAddressOf()),
		L"Failed to create normalWithoutIncinerationTexture!");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	graphics::DeferredShading::getInstance().getGBuffer().normallBuffer.shaderResourceView->GetDesc(&srvDesc);
	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateShaderResourceView(normalWithoutIncinerationTexture.Get(), &srvDesc, normalWithoutIncinerationSRV.ReleaseAndGetAddressOf()),
		L"Failed to create normalWithoutIncinerationTexture!");
}

void MyEngine::spawnEmmiterFromUI()
{
	EmmiterUIData emmiterUIData = UI::getInstance().getNewEmmiterUIData();

	uint32 positionID = utils::SolidVector<graphics::EmmiterPosition>::getInstance().insert(DirectX::XMLoadFloat3(&emmiterUIData.position));
	graphics::ParticleSystem::getInstance().addSmokeEmitter(
		positionID,
		DirectX::XMLoadFloat4(&emmiterUIData.particleColor),
		DirectX::XMLoadFloat2(&emmiterUIData.particleSize),
		DirectX::XMLoadFloat3(&emmiterUIData.particleSpeed),
		emmiterUIData.particleSpawnRatePerSecond,
		emmiterUIData.spawnSphereRadius,
		emmiterUIData.maxParticleNum);
	EmmiterDebugger::getInstance().addDebugger(
		utils::ModelManager::getInstance().getModel(emmiterDebbugingModelPath).value(),
		DirectX::XMLoadFloat4(&emmiterUIData.particleColor),
		emmiterDebuggingModelScale,
		positionID);

	graphics::ParticleSystem::getInstance().updateInstanceBuffer();
	initializeIntersectionSystem();
	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().updateInstanceBuffer();
}

void MyEngine::loadFromJson(const std::string& jsonPath)
{
	int32 width, height;
	MyUtils::getClientSize(renderWindow.GetHWND(), width, height);
	auto j = SceneLoader::getInstance().createJsonFromFile(jsonPath);
	SceneLoader::getInstance().loadCameraFromJson(j, width / static_cast<float32>(height), camera);
	SceneLoader::getInstance().loadBackroundColorFromJson(j, bgcolor);
	SceneLoader::getInstance().loadMeshSystemFromJson(j);
	SceneLoader::getInstance().loadFullscreenTriangleFromJson(j, fullscreenTriangle);
	SceneLoader::getInstance().loadLightSystemFromJson(j);
	SceneLoader::getInstance().loadPostProccesFromJson(j);
	LightDebugger::getInstance().initialize();
	SceneLoader::getInstance().loadLightDebuggerFromJson(j);
	LightDebugger::getInstance().instertLightSystemInstances();
	LightDebugger::getInstance().update();
	diffuseIrradianceCubemap = SceneLoader::getInstance().loadTextureFromJson(j["IBL"], "diffuse_irradiance_cubemap").value();
	specularReflectionCubemap = SceneLoader::getInstance().loadTextureFromJson(j["IBL"], "specular_reflection_cubemap").value();
	specularReflectanceTexture2D = SceneLoader::getInstance().loadTextureFromJson(j["IBL"], "specular_reflectance_texture2D").value();

	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().updateInstanceBuffer();
}

void MyEngine::updateDragging(const window::MouseEvent& me)
{
	if (me.getType() == window::MouseEvent::EventType::LPress)
	{
		startDragging(me);
	}

	if (me.getType() == window::MouseEvent::EventType::LRealese)
	{
		if (dragging)
		{
			pointlightSphereDragging = emmiterSphereDragging = dragging = false;
			ImGuiLog::getInstance().addLog("[INFO] stop dragging\n");
		}
	}

	if (me.getType() == window::MouseEvent::EventType::Move)
	{
		if (dragging)
		{
			moveDraggable(me);
		}
	}
}

void MyEngine::resizeEvent()
{
	int32 width, height;
	MyUtils::getClientSize(renderWindow.GetHWND(), width, height);
	bool byte1Ok = width == 0;
	bool byte2Ok = height == 0;
	if (byte1Ok && byte2Ok)
	{
		//TODO
		return;
	}
	reinitializeRenderTargetView(width, height);
	initializeRasterizer();
	initializeSceneRTV();
	initializeHDRSceneRTV();
	initializeShadowTexturesAndSRVs();
	camera.createProjectionMatrix(camera.getFovRadians(), width / static_cast<float32>(height), camera.getNearZ(), camera.getFarZ());
	initializeSceneDepthTextureSRVCopy();
	graphics::DeferredShading::getInstance().initialize(width, height);
	initializeNormalWithoutIncinerationTextureAndSRV();
	initializePostProcess();
	graphics::DecalSystem::getInstance().initialize(width, height);
}

void MyEngine::reinitializeFromJson(const std::string& jsonPath)
{
	try
	{
		loadFromJson(jsonPath);
		fullscreenTriangle.update(3, camera);
		initializeIntersectionSystem();
		initializeShadowTexturesAndSRVs();
		initalizeDissolutionShadingGroup();
		graphics::MeshSystem::getInstance().updateModelInstancesIDs();
		graphics::MeshSystem::getInstance().getOpaqueInstances().updateInstanceBuffer();
		ImGuiLog::getInstance().addLog("[Successful] Scene is loaded from json\"%s\"\n", UI::getInstance().getSceneFilePath());
	}
	catch (std::exception& e)
	{
		ImGuiLog::getInstance().addLog("[Error] Can't load scene from json \"%s\". File doesn't exist or json is incorrect.\n", UI::getInstance().getSceneFilePath());
		ImGuiLog::getInstance().addLog("[Error] Exception: %s\n", e.what());
	}
}

void MyEngine::updateCamerFlashlight()
{
	auto lightsCBS = graphics::LightSystem::getInstance().getLightCBS();
	XMStoreFloat4(&lightsCBS.spotLightsCBS[0].position, camera.getPositionVector());
	XMStoreFloat4(&lightsCBS.spotLightsCBS[0].direction, camera.getForwardVector());
	graphics::LightSystem::getInstance().setLightCBS(lightsCBS);
	//LightDebugger::getInstance().update();
}

void MyEngine::startDragging(const window::MouseEvent& me)
{
	float32 u, v;
	MyUtils::getInstance().getClientUV(renderWindow.GetHWND(), me.getPosX(), me.getPosY(), u, v);
	math::RRay worldRay = createLookAtRay(u, v);

	currentIntersectionSystemRec.intersectionRec.t = std::numeric_limits<float32>::max();
	if (IntersectionSystem::getInstance().intersect(worldRay, currentIntersectionSystemRec))
	{
		if (LightDebugger::getInstance().isPointlightInstanceID(currentIntersectionSystemRec.instanceID))
			pointlightSphereDragging = true;
		if (EmmiterDebugger::getInstance().isEmmiterInstanceID(currentIntersectionSystemRec.instanceID))
			emmiterSphereDragging = true;
		dragging = true;
		ImGuiLog::getInstance().addLog("[INFO] start dragging\n");
	}
}

void MyEngine::moveDraggable(const window::MouseEvent& me)
{
	float32 u, v;
	MyUtils::getInstance().getClientUV(renderWindow.GetHWND(), me.getPosX(), me.getPosY(), u, v);
	math::RRay worldRay = createLookAtRay(u, v);

	const DirectX::XMVECTOR vertex = worldRay.at(currentIntersectionSystemRec.intersectionRec.t);
	const DirectX::XMVECTOR offset = vertex - currentIntersectionSystemRec.intersectionRec.position;
	const DirectX::XMMATRIX offsetMatrix = DirectX::XMMatrixTranslation(DirectX::XMVectorGetX(offset), DirectX::XMVectorGetY(offset), DirectX::XMVectorGetZ(offset));
	const DirectX::XMMATRIX modelToWorldMat = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&utils::SolidVector<graphics::MeshSystem::OpaqueInstanceGPU>::getInstance()[currentIntersectionSystemRec.instanceID].modelToWorld));
	DirectX::XMFLOAT4X4 newModelToWorld;
	{
		DirectX::XMStoreFloat4x4(&newModelToWorld, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(modelToWorldMat, offsetMatrix)));
	}
	utils::SolidVector<graphics::MeshSystem::OpaqueInstanceGPU>::getInstance()[currentIntersectionSystemRec.instanceID].modelToWorld = newModelToWorld;
	currentIntersectionSystemRec.intersectionRec.position = vertex;

	if (pointlightSphereDragging)
	{
		LightDebugger::getInstance().changePointlight(currentIntersectionSystemRec.instanceID);
		graphics::LightSystem::getInstance().setLightCBS(graphics::LightSystem::getInstance().getLightCBS());
	}
	if (emmiterSphereDragging)
	{
		EmmiterDebugger::getInstance().changeEmmiterPosition(currentIntersectionSystemRec.instanceID);
	}

	graphics::MeshSystem::getInstance().getOpaqueInstances().updateInstanceBuffer();
	graphics::MeshSystem::getInstance().getDebugOpaqueInstances().updateInstanceBuffer();
	graphics::DecalSystem::getInstance().updateInstanceBuffer();
}

void MyEngine::spawnDecalInstance()
{
	math::RRay worldRay = createLookAtRay(0.5f, 0.5f);
	IntersectionSystem::IntersectionSystemRecord rec;

	rec.intersectionRec.t = std::numeric_limits<float32>::max();
	if (IntersectionSystem::getInstance().intersect(worldRay, rec))
	{
		graphics::MeshSystem::OpaqueInstanceGPU opaqueInstance = utils::SolidVector<graphics::MeshSystem::OpaqueInstanceGPU>::getInstance()[rec.instanceID];
		DirectX::XMFLOAT4X4 modelToWorld = opaqueInstance.modelToWorld;
		DirectX::XMMATRIX worldToModel = XMMatrixInverse(nullptr, XMMatrixTranspose(XMLoadFloat4x4(&opaqueInstance.modelToWorld)));
		
		const float32 angle = utils::RandomGenerator::getInstance().randomFloat(0.f, DirectX::XM_2PI);
		const float32 colorScale = 3.f;
		const float32 scale = 0.25f;

		XMMATRIX decalBasis;
		decalBasis.r[0] = XMVectorSetW(camera.getRightVector(), 0.f);
		decalBasis.r[1] = XMVectorSetW(camera.getTopVector(), 0.f);
		decalBasis.r[2] = XMVectorSetW(camera.getForwardVector(), 0.f);
		decalBasis.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		XMMATRIX scaleMat = XMMatrixScaling(scale, scale, scale);
		XMMATRIX rotateMat = XMMatrixRotationAxis(camera.getForwardVector(), angle);

		XMVECTOR offsetInModelSpace = XMVector3Transform(rec.intersectionRec.position, worldToModel);

		graphics::DecalInstanceCPU instanceCPU;
		instanceCPU.color = XMVectorSet(
			colorScale * utils::RandomGenerator::getInstance().randomFloat(0.f, 1.f),
			colorScale * utils::RandomGenerator::getInstance().randomFloat(0.f, 1.f),
			colorScale * utils::RandomGenerator::getInstance().randomFloat(0.f, 1.f),
			1.f);
		instanceCPU.scaledRotatedDecalBasis = decalBasis * rotateMat * scaleMat;
		instanceCPU.instanceID = rec.instanceID;
		instanceCPU.offsetInModelSpace = offsetInModelSpace;

		auto& decalInstances = graphics::DecalSystem::getInstance().getDecalInstances();
		decalInstances.push_back(instanceCPU);
		graphics::DecalSystem::getInstance().setDecalInstances(decalInstances);
		graphics::DecalSystem::getInstance().updateInstanceBuffer();

		ImGuiLog::getInstance().addLog("[INFO] spawn decal\n");
	}
}

void MyEngine::updateDissolutionShadingGroupInstances(float64 elapsed)
{
	graphics::MeshSystem::DissolutionInstance* data = utils::SolidVector<graphics::MeshSystem::DissolutionInstance>::getInstance().data();
	uint32 dataSize = utils::SolidVector<graphics::MeshSystem::DissolutionInstance>::getInstance().size();

	for (int i = 0; i < dataSize; ++i)
	{
		data[i].time += elapsed;
	}
}

void MyEngine::updateIncinerationShadingGroupInstances(float64 elapsed)
{
	graphics::MeshSystem::IncinerationInstance* data = utils::SolidVector<graphics::MeshSystem::IncinerationInstance>::getInstance().data();
	uint32 dataSize = utils::SolidVector<graphics::MeshSystem::IncinerationInstance>::getInstance().size();

	for (int i = 0; i < dataSize; ++i)
	{
		data[i].prevBoundingShpereRadius = (data[i].time / incinerationInstanceLifetime) * data[i].maxBoundingShpereRadius;
		data[i].time += elapsed;
		data[i].currentBoundingShpereRadius = (data[i].time / incinerationInstanceLifetime) * data[i].maxBoundingShpereRadius;
	}
}

math::RRay MyEngine::createLookAtRay(float32 u, float32 v)
{
	math::RFrustum frustum;
	frustum.create(camera);

	const DirectX::XMVECTOR origin = camera.getPositionVector();
	const DirectX::XMVECTOR horizontal = frustum.getNearHorizontal();
	const DirectX::XMVECTOR vertical = frustum.getNearVertical();
	const DirectX::XMVECTOR lowerLeftCorner = frustum.getNearLeftTop();

	const math::RRay worldRay(origin, DirectX::XMVector3Normalize(lowerLeftCorner + horizontal * u - vertical * v - origin));
	return worldRay;
}

void MyEngine::prerenderSceneRTV(const std::array<float32, 4>& color)
{
	d3d::devcon->RSSetState(rasterizerState.Get());
	d3d::devcon->ClearRenderTargetView(sceneRTV.Get(), color.data());
	d3d::devcon->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
}

void MyEngine::bindConstantBuffers()
{
	int32 width, height;
	MyUtils::getClientSize(renderWindow.GetHWND(), width, height);
	plasmaConstantBuffer.data.resolution.x = static_cast<float32>(width);
	plasmaConstantBuffer.data.resolution.y = static_cast<float32>(height);
	plasmaConstantBuffer.data.time = static_cast<float32>(MyUtils::getInstance().secondsSinceStart());
	plasmaConstantBuffer.applyChanges(PlasmaConstantBufferStruct::shaderRegister, graphics::MapTarget::VS | graphics::MapTarget::PS | graphics::MapTarget::GS);

	math::RFrustum cameraFrustum;
	cameraFrustum.create(camera);
	cameraConstantBuffer.data.clipToView = camera.getProjectionInvFloat4x4();
	cameraConstantBuffer.data.viewToWorld = camera.getViewInvFloat4x4();
	cameraConstantBuffer.data.viewToClip = camera.getProjectionFloat4x4();
	cameraConstantBuffer.data.worldToView = camera.getViewFloat4x4();
	cameraConstantBuffer.data.cameraPosition = camera.getPositionFloat3();
	DirectX::XMStoreFloat3(&cameraConstantBuffer.data.cameraVertical, cameraFrustum.getNearVertical());
	DirectX::XMStoreFloat3(&cameraConstantBuffer.data.cameraHorizontal, cameraFrustum.getNearHorizontal());
	DirectX::XMStoreFloat3(&cameraConstantBuffer.data.cameraLeftTopCorner, cameraFrustum.getNearLeftTop());

	const XMMATRIX IDMATRIX = XMMatrixMultiply(camera.getProjectionMatrix(), camera.getProjectionInvMatrix());

	cameraConstantBuffer.applyChanges(CameraConstantBufferStruct::shaderRegister, graphics::MapTarget::VS | graphics::MapTarget::PS | graphics::MapTarget::GS | graphics::MapTarget::CS);

	graphics::LightSystem::getInstance().bind(4, graphics::MapTarget::PS);

	textureViewConstantBuffer.data.sampleStateIndex = UI::getInstance().getViewTextureFilter();
	textureViewConstantBuffer.applyChanges(TextureViewConstantBufferStruct::shaderRegister, graphics::MapTarget::PS);

	IBLCB.data.E_diff = UI::getInstance().getIBLEDiff();
	IBLCB.data.E_direct = UI::getInstance().getIBLEDirect();
	IBLCB.data.E_spec = UI::getInstance().getIBLESpec();
	IBLCB.applyChanges(IBLCBS::shaderRegister, graphics::PS);
}

void MyEngine::bindSamplerStates()
{
	d3d::devcon->PSSetSamplers(0, 1, samplerStates[UI::getInstance().getViewTextureFilter()].GetAddressOf());
	d3d::devcon->PSSetSamplers(1, 1, samplerStateDepthAnisotropic.GetAddressOf());
	d3d::devcon->CSSetSamplers(0, 1, samplerStates[UI::getInstance().getViewTextureFilter()].GetAddressOf());
}

void MyEngine::updateSpotlightShadowMapCB(const graphics::SpotLightCBS& spotlightCB, float32 textureSize)
{
	const auto xaxis = XMVector3Cross(XMVectorSet(0, 1, 0, 0), XMLoadFloat4(&spotlightCB.direction));
	const auto nxaxis = XMVector3Normalize(xaxis);

	const auto yaxis = XMVector3Cross(XMLoadFloat4(&spotlightCB.direction), xaxis);
	const auto nyaxis = XMVector3Normalize(yaxis);

	DirectX::XMVECTOR upDir = nyaxis;
	DirectX::XMVECTOR eyePos = XMLoadFloat4(&spotlightCB.position);
	DirectX::XMVECTOR focusPos = XMLoadFloat4(&spotlightCB.direction) + eyePos;

	auto modelToLight = DirectX::XMMatrixLookAtLH(eyePos, focusPos, upDir);
	auto lightToClip = XMMatrixPerspectiveFovLH(2.f * spotlightCB.angle.x, 1.0f, spotlightCB.radius.x, 0.1f);
	
	float32 texelWorldSize = 2.f * XMScalarSin(spotlightCB.angle.x) / textureSize;

	DirectX::XMStoreFloat4x4(&shadowMap2DCB.data.worldToLightView, modelToLight);
	DirectX::XMStoreFloat4x4(&shadowMap2DCB.data.lightViewToClip, lightToClip);
	shadowMap2DCB.data.lightPosition = { spotlightCB.position.x, spotlightCB.position.y, spotlightCB.position.z };
	shadowMap2DCB.data.texelWorldSize = texelWorldSize;
}

void MyEngine::updateDirectionallightShadowMapCB(const graphics::DirectionalLightsCBS& directionallightCB, float32 textureSize)
{
	const auto xaxis = XMVector3Cross(XMVectorSet(0, 1, 0, 0), XMLoadFloat4(&directionallightCB.direction));
	const auto nxaxis = XMVector3Normalize(xaxis);

	const auto yaxis = XMVector3Cross(XMLoadFloat4(&directionallightCB.direction), xaxis);
	const auto nyaxis = XMVector3Normalize(yaxis);

	const auto position = -15.f * XMLoadFloat4(&directionallightCB.direction);

	DirectX::XMVECTOR upDir = nyaxis;
	DirectX::XMVECTOR eyePos = position;
	DirectX::XMVECTOR focusPos = XMLoadFloat4(&directionallightCB.direction) + eyePos;

	auto modelToLight = DirectX::XMMatrixLookAtLH(eyePos, focusPos, upDir);
	auto lightToClip = XMMatrixOrthographicLH(10.f, 10.f, 100.f, 0.001f);

	float32 texelWorldSize = 10.f / textureSize;

	DirectX::XMStoreFloat4x4(&shadowMap2DCB.data.worldToLightView, modelToLight);
	DirectX::XMStoreFloat4x4(&shadowMap2DCB.data.lightViewToClip, lightToClip);
	DirectX::XMStoreFloat3(&shadowMap2DCB.data.lightPosition, position);
	shadowMap2DCB.data.texelWorldSize = texelWorldSize;
}

void MyEngine::createShadowDepthStencilView(Microsoft::WRL::ComPtr<ID3D11Texture2D>& shadowMapTexture)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	HRESULT hr = d3d::device->CreateDepthStencilView(shadowMapTexture.Get(), &descDSV, shadowDepthStencilView.ReleaseAndGetAddressOf());
}

void MyEngine::updatePointlightShadowMapCB(const graphics::PointLightsCBS& pointlightCB, float32 textureSize)
{
	const DirectX::XMVECTOR position = XMLoadFloat4(&pointlightCB.position);
	const std::array<DirectX::XMMATRIX, 6> worlToViewMatArr =
	{
		DirectX::XMMatrixLookAtLH(position, position + XMVECTOR{ 1.f, 0.f, 0.f}, { 0.f, 1.f, 0.f}),
		DirectX::XMMatrixLookAtLH(position, position + XMVECTOR{-1.f, 0.f, 0.f}, { 0.f, 1.f, 0.f}),
		DirectX::XMMatrixLookAtLH(position, position + XMVECTOR{ 0.f, 1.f, 0.f}, { 0.f, 0.f,-1.f}),
		DirectX::XMMatrixLookAtLH(position, position + XMVECTOR{ 0.f,-1.f, 0.f}, { 0.f, 0.f, 1.f}),
		DirectX::XMMatrixLookAtLH(position, position + XMVECTOR{ 0.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}),
		DirectX::XMMatrixLookAtLH(position, position + XMVECTOR{ 0.f, 0.f,-1.f}, { 0.f, 1.f, 0.f})
	};

	const DirectX::XMMATRIX projMat = XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, pointlightCB.radius.x, 0.1f);
	DirectX::XMStoreFloat4x4(&shadowMap3DCB.data.lightViewToClip, projMat);

	shadowMap3DCB.data.texelWorldSize = 2.f / textureSize;

	DirectX::XMStoreFloat3(&shadowMap3DCB.data.lightPosition, position);

	for (int face = 0; face < 6; ++face)
	{
		DirectX::XMStoreFloat4x4(&shadowMap3DCB.data.worldToLightViewsPerFace[face], worlToViewMatArr[face]);
	}
}

void MyEngine::createShadow3DDepthStencilView(Microsoft::WRL::ComPtr<ID3D11Texture2D>& shadowMapTexture)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	descDSV.Texture2DArray.MipSlice = 0;
	descDSV.Texture2DArray.FirstArraySlice = 0;
	descDSV.Texture2DArray.ArraySize = 6;
	HRESULT hr = d3d::device->CreateDepthStencilView(shadowMapTexture.Get(), &descDSV, shadowDepthStencilView.ReleaseAndGetAddressOf());
}

void MyEngine::deleteOpaqueInstanceHandler()
{
	math::RRay worldRay = createLookAtRay(0.5f, 0.5f);
	IntersectionSystem::IntersectionSystemRecord rec;
	IntersectionSystem::ModelIntersectionSystemRecord modelRec;

	rec.intersectionRec.t = std::numeric_limits<float32>::max();
	if (IntersectionSystem::getInstance().intersectModel(worldRay, rec, modelRec))
	{
		graphics::MeshSystem::OpaqueInstanceGPU opaqueInstance = utils::SolidVector<graphics::MeshSystem::OpaqueInstanceGPU>::getInstance()[modelRec.instanceID];
		graphics::MeshInstanceID meshInstanceID;
		meshInstanceID.instanceID = rec.instanceID;
		meshInstanceID.materialID = rec.materialID;
		meshInstanceID.modelID = rec.modelID;
		meshInstanceID.shadingID = 0u;

		//DirectX::XMVECTOR aaabSize = rec.model->aabb.max - rec.model->aabb.max;

		graphics::MeshSystem::IncinerationInstance incinerationInstance;
		incinerationInstance.color.x = utils::RandomGenerator::getInstance().randomFloat(0.f, 1.f);
		incinerationInstance.color.y = utils::RandomGenerator::getInstance().randomFloat(0.f, 1.f);
		incinerationInstance.color.z = utils::RandomGenerator::getInstance().randomFloat(0.f, 1.f);
		incinerationInstance.modelToWorld = opaqueInstance.modelToWorld;
		DirectX::XMStoreFloat3(&incinerationInstance.rayIntersection, rec.intersectionRec.position);
		incinerationInstance.currentBoundingShpereRadius = 0.f;
		incinerationInstance.prevBoundingShpereRadius = 0.f;
		incinerationInstance.maxBoundingShpereRadius = DirectX::XMVectorGetX(DirectX::XMVector3Length(rec.model->aabb.max - rec.model->aabb.min));
		incinerationInstance.time = 0.f;
		//incinerationInstance.boundingShpereRadius = std::max(XMVectorGetX(aaabSize), XMVectorGetY(aaabSize), XMVectorGetZ(aaabSize)) / 2.f;

		uint32 newInstanceID = utils::SolidVector<graphics::MeshSystem::IncinerationInstance>::getInstance().insert(incinerationInstance);

		const auto& perModelArr = graphics::MeshSystem::getInstance().getOpaqueInstances().getPerModelArr();
		std::vector<graphics::Material> materials(modelRec.perMeshMaterialIDs.size());
		for (uint32 meshIndex = 0u; meshIndex < modelRec.perMeshMaterialIDs.size(); ++meshIndex)
		{
			const auto& perMesh = perModelArr[modelRec.modelID].perMeshArr[meshIndex];
			const auto& perMaterial = perMesh.perMaterialArr[modelRec.perMeshMaterialIDs[meshIndex]];
			materials[meshIndex] = perMaterial.material;
		}
		MyUtils::insertModelInstanceIDInShadingGroup(graphics::MeshSystem::getInstance().getIncinerationShadingGroup(), newInstanceID, modelRec.model, materials);

		graphics::MeshSystem::getInstance().remove(0u, modelRec.modelID, modelRec.perMeshMaterialIDs, modelRec.instanceID);
		graphics::MeshSystem::getInstance().updateModelInstancesIDs();
		graphics::MeshSystem::getInstance().getOpaqueInstances().updateInstanceBuffer();
		graphics::DecalSystem::getInstance().updateInstanceBuffer();
		initializeIntersectionSystem();

		ImGuiLog::getInstance().addLog("[INFO] delete instance\n");
	}

	graphics::MeshSystem::getInstance().getIncinerationShadingGroup().updateInstanceBuffer();
}

void MyEngine::spawnModelInFrontOfCamera(const float32 distance)
{
	uint32 newInstanceID = utils::SolidVector<graphics::MeshSystem::DissolutionInstance>::getInstance().insert(createDissolutionInstanceInFrontOfCamera(distance));

	const auto& randomOpaquePerModel = graphics::MeshSystem::getInstance().getOpaqueInstances().getPerModelArr().front();

	const std::shared_ptr<graphics::Model>& newDissolutionModel = randomOpaquePerModel.model;

	for (int i = 0; i < randomOpaquePerModel.perMeshArr.size(); ++i)
	{
		auto& randomMaterial = randomOpaquePerModel.perMeshArr[i].perMaterialArr.front().material;
		MyUtils::insertInstanceIDInShadingGroup(graphics::MeshSystem::getInstance().getDissolutionShadingGroup(), newInstanceID, newDissolutionModel, randomMaterial);
	}


	graphics::MeshSystem::getInstance().getDissolutionShadingGroup().updateInstanceBuffer();
}

graphics::MeshSystem::DissolutionInstance MyEngine::createDissolutionInstanceInFrontOfCamera(float32 distance)
{
	DirectX::XMVECTOR position = camera.getPositionVector() + distance * camera.getForwardVector();
	DirectX::XMMATRIX translationMat = XMMatrixTranslation(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));

	graphics::MeshSystem::DissolutionInstance instance;
	XMStoreFloat4(&instance.color, XMVectorSet(1, 1, 1, 0));
	XMStoreFloat4x4(&instance.modelToWorld, XMMatrixTranspose(translationMat));
	instance.time = 0.f;

	return instance;
}

void MyEngine::updateOutdatedDissolutionInstances()
{
	auto& perModelArr = graphics::MeshSystem::getInstance().getDissolutionShadingGroup().getPerModelArr();
	for (int i = 0; i < perModelArr.size(); ++i)
	{
		auto& perModel = perModelArr[i];
		for (int j = 0; j < perModel.perMeshArr.size(); ++j)
		{
			auto& perMesh = perModel.perMeshArr[j];
			for (int k = 0; k < perMesh.perMaterialArr.size(); ++k)
			{
				auto& perMaterial = perMesh.perMaterialArr[k];
				for (int m = 0; m < perMaterial.instancesID.size(); ++m)
				{
					uint32 dissolutionInstanceID = perMaterial.instancesID[m];
					graphics::MeshSystem::DissolutionInstance& dissolutionInstance = utils::SolidVector<graphics::MeshSystem::DissolutionInstance>::getInstance()[dissolutionInstanceID];

					if (dissolutionInstance.time > dissolutionInstanceLifetime)
					{
						graphics::MeshSystem::OpaqueInstanceGPU opaqueInstance;
						opaqueInstance.color = dissolutionInstance.color;
						opaqueInstance.modelToWorld = dissolutionInstance.modelToWorld;
						uint32 opaqueInstanceID = utils::SolidVector<graphics::MeshSystem::OpaqueInstanceGPU>::getInstance().insert(static_cast<graphics::MeshSystem::OpaqueInstanceGPU>(dissolutionInstance));
						MyUtils::insertInstanceIDInShadingGroup(graphics::MeshSystem::getInstance().getOpaqueInstances(), opaqueInstanceID, perModel.model, perMaterial.material);
						
						perMaterial.instancesID.erase(perMaterial.instancesID.begin() + m);

						utils::SolidVector<graphics::MeshSystem::DissolutionInstance>::getInstance().erase(dissolutionInstanceID);
					}
				}
				if (perMaterial.instancesID.empty())
					perMesh.perMaterialArr.erase(perMesh.perMaterialArr.begin() + k);
			}
			if (perMesh.perMaterialArr.empty())
				perModel.perMeshArr.erase(perModel.perMeshArr.begin() + j);
		}
		if (perModel.perMeshArr.empty())
			perModelArr.erase(perModelArr.begin() + i);
	}
}

// !TODO okay this is very bad code, but i want to complete this hw faster
void MyEngine::updateOutdatedIncinerationInstances()
{
	std::vector<uint32> outdatedInstances;

	auto& perModelArr = graphics::MeshSystem::getInstance().getIncinerationShadingGroup().getPerModelArr();
	for (int i = 0; i < perModelArr.size(); ++i)
	{
		auto& perModel = perModelArr[i];
		for (int j = 0; j < perModel.perMeshArr.size(); ++j)
		{
			auto& perMesh = perModel.perMeshArr[j];
			for (int k = 0; k < perMesh.perMaterialArr.size(); ++k)
			{
				auto& perMaterial = perMesh.perMaterialArr[k];
				for (int m = 0; m < perMaterial.instancesID.size(); ++m)
				{
					uint32 incinerationInstanceID = perMaterial.instancesID[m];
					graphics::MeshSystem::IncinerationInstance& incinerationInstance = utils::SolidVector<graphics::MeshSystem::IncinerationInstance>::getInstance()[incinerationInstanceID];

					if (incinerationInstance.time > incinerationInstanceLifetime)
					{
						perMaterial.instancesID.erase(perMaterial.instancesID.begin() + m);
					}
				}
				if (perMaterial.instancesID.empty())
					perMesh.perMaterialArr.erase(perMesh.perMaterialArr.begin() + k);
			}
			if (perMesh.perMaterialArr.empty())
				perModel.perMeshArr.erase(perModel.perMeshArr.begin() + j);
		}
		if (perModel.perMeshArr.empty())
			perModelArr.erase(perModelArr.begin() + i);
	}

	for (auto& outdateInstance : outdatedInstances)
		utils::SolidVector<graphics::MeshSystem::IncinerationInstance>::getInstance().erase(outdateInstance);
}

void MyEngine::setClientSizeViewport()
{
	int width, height;
	MyUtils::getClientSize(renderWindow.GetHWND(), width, height);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float32>(width);
	viewport.Height = static_cast<float32>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	d3d::devcon->RSSetViewports(1, &viewport);
}

LRESULT MyEngine::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (UI::getInstance().updateWindowEvents(hwnd, uMsg, wParam, lParam))
	{
		return true;
	}
	return windowProcEvents(hwnd, uMsg, wParam, lParam);
}