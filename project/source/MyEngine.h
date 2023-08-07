#pragma once

#include <engine/Engine.h>
#include <engine/graphics/LightSystem.h>
#include <engine/math/RFrustum.h>
#include <engine/graphics/PostProcess.h>
#include <engine/graphics/ParticleSystem.h>
#include <engine/graphics/DecalSystem.h>
#include <limits>
#include <locale>
#include <codecvt>
#include "UI.h"
#include "MyUtils.h"
#include "CameraController.h"
#include "IntersectionSystem.h"
#include "FullscreenTriangle.h"
#include "LightDebugger.h"
#include "EmmiterDebugger.h"
#include <engine/utils/ReflectionCapture.h>
#include <DirectXTex/DirectXTex.h>
#include <engine/graphics/DeferredShading.h>
#include <engine/graphics/FullscreenPass.h>

#undef max

struct PlasmaConstantBufferStruct
{
	static constexpr int32 shaderRegister = 0;
	float time;
	DirectX::XMFLOAT3 padding1;
	DirectX::XMFLOAT4 resolution;
};

struct CameraConstantBufferStruct
{
	static constexpr int32 shaderRegister = 1;
	XMFLOAT3 cameraPosition;
	float32 padding1;
	XMFLOAT3 cameraVertical;
	float32 padding2;
	XMFLOAT3 cameraHorizontal;
	float32 padding3;
	XMFLOAT3 cameraLeftTopCorner;
	float32 padding4;
	XMFLOAT4X4 worldToView;
	XMFLOAT4X4 viewToClip;
	XMFLOAT4X4 clipToView;
	XMFLOAT4X4 viewToWorld;
};

struct TextureViewConstantBufferStruct
{
	static constexpr int32 shaderRegister = 2;
	float32 sampleStateIndex;
	XMFLOAT3 padding;
};

struct IBLCBS
{
	static constexpr int32 shaderRegister = 10;
	uint32 E_direct;
	XMFLOAT3 _pad1;
	uint32 E_diff;
	XMFLOAT3 _pad2;
	uint32 E_spec;
	XMFLOAT3 _pad3;
};

struct ShadowMapArrayConstantBuffer
{
	static constexpr uint32 shaderRegister = 12u;
	graphics::ShadowMap2DConstantBuffer shadowMaps2DCBs[8];
	graphics::ShadowMap3DConstantBuffer shadowMaps3DCBs[8];
};

class MyEngine : public engine::Engine
{
public:
	static void d3dinit();
	static void d3ddeinit();

public:
	void initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int32 width, int32 height);
	bool processMessages();
	void update(float64 elapsed);
	void renderFrame(float64 elapsed);

private:
	void initializeSwapchain(HWND hwnd);
	void initializeBackBuffer();
	void initializeDepthBuffer();
	void initializeRenderTargetView();
	void initializeViewport(int32 width, int32 height);
	void reinitializeRenderTargetView(int32 width, int32 height);
	void initializeConstantBuffers();
	void initializeIntersectionSystem();
	void initializeDebugShaders();
	void initializeSamplerState();
	void initializeRasterizer();
	void initializeSceneRTV();
	void initializeHDRSceneRTV();
	void initializeShadowTexturesAndSRVs();
	void initalizeDissolutionShadingGroup();
	void initalizeIncinerationShadingGroup();
	void initializeSceneBlendState();
	void initializeSceneDepthTextureSRVCopy();
	void initializeParticelSystem();
	void initializeSceneDepthStencilState();
	void initializePostProcess();
	void initializeNormalWithoutIncinerationTextureAndSRV();
	void loadFromJson(const std::string& jsonPath);

	void updateCamerFlashlight();
	void reinitializeFromJson(const std::string& jsonPath);
	void resizeEvent();

	void spawnModelInFrontOfCamera(const float32 distance);

	void updateDragging(const window::MouseEvent& me);
	void startDragging(const window::MouseEvent& me);
	void moveDraggable(const window::MouseEvent& me);

	void spawnDecalInstance();

	void updateDissolutionShadingGroupInstances(float64 elapsed);
	void updateIncinerationShadingGroupInstances(float64 elapsed);

	void prerenderSceneRTV(const std::array<float32, 4>& color);
	void preRender();
	void postRender();
	void bindConstantBuffers();
	void bindSamplerStates();
	void renderDebug();
	void renderShadowMaps();
	void renderSpotlightShadowMaps();
	void renderSunlightShadowMaps();
	void renderPointlightShadowMaps();

	void setClientSizeViewport();
	void updateSpotlightShadowMapCB(const graphics::SpotLightCBS& spotlightCB, float32 textureSize);
	void updateDirectionallightShadowMapCB(const graphics::DirectionalLightsCBS& directionallightCB, float32 textureSize);
	void updatePointlightShadowMapCB(const graphics::PointLightsCBS& pointlightCB, float32 textureSize);
	void createShadowDepthStencilView(Microsoft::WRL::ComPtr<ID3D11Texture2D>& shadowMapTexture);
	void createShadow3DDepthStencilView(Microsoft::WRL::ComPtr<ID3D11Texture2D>& shadowMapTexture);

	void deleteOpaqueInstanceHandler();

	void spawnEmmiterFromUI();

	math::RRay createLookAtRay(float32 u, float32 v);

	graphics::MeshSystem::DissolutionInstance createDissolutionInstanceInFrontOfCamera(float32 distance);
	void updateOutdatedDissolutionInstances();
	void updateOutdatedIncinerationInstances();

	void addEmmiterWithDebugger(
		const uint32 positionID,
		const DirectX::XMVECTOR& spawnedParticleColor,
		const DirectX::XMVECTOR& spawnParticleSize,
		const DirectX::XMVECTOR& spawnParticleSpeed,
		float32 spawnRatePerSecond,
		float32 spawnCircleRadius,
		uint32 maxParticlesNum);

private:
	LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
	D3D11_TEXTURE2D_DESC backbufferDesc;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> sceneTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sceneSRV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> hdrSceneTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> hdrSceneSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> hdrSceneRTV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shadowsMaps2DSRVs;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shadowsMaps3DSRVs;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> sceneRTV;
	std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, 3> samplerStates;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStateDepthAnisotropic;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> shadowMapTextures2D;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> shadowMapTextures3D;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBufferCopy;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthCopyStencilViewSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStateFill;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStateEqual;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthWithoutIncinerationTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthWithoutIncinerationSRV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> normalWithoutIncinerationTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalWithoutIncinerationSRV;

	graphics::PipelineShaders normalDebugShaders;
	graphics::PipelineShaders wireframeDebugShaders;

	graphics::ConstantBuffer<graphics::ShadowMap2DConstantBuffer> shadowMap2DCB;
	graphics::ConstantBuffer<graphics::ShadowMap3DConstantBuffer> shadowMap3DCB;
	graphics::ConstantBuffer<CameraConstantBufferStruct> cameraConstantBuffer;
	graphics::ConstantBuffer<PlasmaConstantBufferStruct> plasmaConstantBuffer;
	graphics::ConstantBuffer<IBLCBS> IBLCB;
	graphics::ConstantBuffer<TextureViewConstantBufferStruct> textureViewConstantBuffer;
	graphics::ConstantBuffer<ShadowMapArrayConstantBuffer> shadowMapsArrayCBs;

	std::shared_ptr<graphics::Texture> diffuseIrradianceCubemap;
	std::shared_ptr<graphics::Texture> specularReflectionCubemap;
	std::shared_ptr<graphics::Texture> specularReflectanceTexture2D;

	std::shared_ptr<graphics::Texture> flashlightRextureMask;
	std::shared_ptr<graphics::Texture> fireNoiseTexture;
	FullscreenTriangle fullscreenTriangle;
	math::RCamera camera;
	CameraController cameraController;
	window::MousePoint oldMousePoint{};

	IntersectionSystem::IntersectionSystemRecord currentIntersectionSystemRec;

	bool dragging = false;
	bool pointlightSphereDragging = false;
	bool emmiterSphereDragging = false;

	std::array<float, 4> bgcolor{};

private:
	static constexpr float32 minDepth = 0.0f;
	static constexpr float32 maxDepth = 1.0f;

private:
	static constexpr uint32 shadowMapTexturesSpotlightNum = 4u;
	static constexpr uint32 shadowMapTexturesDirectionalLightNum = 4u;
	static constexpr uint32 shadowMapTexturesPointlightNum = 8u;
	static constexpr uint32 directionallightSize = 2 * 2048u;
	static constexpr uint32 spotlightSize = 1024u;
	static constexpr uint32 pointlightSize = 1024u;
	static constexpr uint32 meshStartSlot = 2u;
	static constexpr uint32 meshMapTarget = graphics::MapTarget::VS;
	static constexpr uint32 materialStartSlot = 3u;
	static constexpr uint32 materialMapTarget = graphics::MapTarget::PS;
	static constexpr uint32 albedoStartSlot = 0u;
	static constexpr uint32 metallicStartSlot = 1u;
	static constexpr uint32 roughnessStartSlot = 2u;
	static constexpr uint32 normalMapStartSlot = 3u;
	static constexpr float32 modelSpawnDistance = 5.f;
	static constexpr float64 dissolutionTimeUpdateRate = 0.01;
	static constexpr float64 dissolutionInstanceLifetime = 1.0;
	static constexpr float64 incinerationInstanceLifetime = 1.0;
	static constexpr float64 rangeBufferStartSlot = 10u;
	static constexpr const wchar_t* incierationShadingGroupVertexShaderFilename = L"incineration_vertex.cso";
	static constexpr const wchar_t* incierationShadingGroupPixelShaderFilename = L"incineration_pixel.cso";
	static constexpr const wchar_t* dissolutionShadingGroupVertexShaderFilename = L"dissolution_vertex.cso";
	static constexpr const wchar_t* dissolutionShadingGroupPixelShaderFilename = L"dissolution_pixel.cso";
	static constexpr const char* emmiterDebbugingModelPath = "../resources/models/emmiter_debug_model.fbx";
	static constexpr const float emmiterDebuggingColor[4] = { 1.f, 1.f, 1.f, 1.f };
	static constexpr const float emmiterDebuggingModelScale = 0.05f;
	static constexpr const uint32 MAX_GPU_PARTICLES_NUM = 100u;// 162678u;
};