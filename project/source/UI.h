#pragma once

#include <engine/graphics/MeshSystem.h>
#include <engine/graphics/PostProcess.h>
#include "IntersectionSystem.h"
#include <engine/graphics/ParticleSystem.h>

#include "ImGuiLog.h"

#include "SceneLoader.h"

#include <queue>

struct EmmiterUIData
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 particleColor;
	DirectX::XMFLOAT2 particleSize;
	DirectX::XMFLOAT3 particleSpeed;
	float32 particleSpawnRatePerSecond;
	float32 spawnSphereRadius;
	int32 maxParticleNum;
};

class UI
{
public:
	class Event
	{
	public:
		enum Type
		{
			None,
			LoadScene,
			ViewShadersCheckbox,
			LightDebuggerUpdated,
			SpawnEmmiter
		};
	public:
		Event(const Type& type) : type(type) {}
		const Type& getType() { return type; }
	private:
		Type type;
	};
public:
	static UI& getInstance();
public:
	void initialize(HWND hWnd);
	void render();
	bool updateWindowEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	Event readEvent();
	bool eventsBufferIsEmpty();

	const char* getSceneFilePath() const;
	void setViewSceneShaders(bool val);
	void setViewNormalShaders(bool val);
	void setViewWireframeShaders(bool val);
	void setViewTextureFilter(int32 val);
	void setPostprocessEv100Index(int32 val);
	bool getViewSceneShaders() const;
	bool getViewNormalShaders() const;
	bool getViewWireframeShaders() const;
	int32 getViewTextureFilter() const;
	bool getDetachFlashlightFromCamera() const;
	int32 getPostprocessEv100Index() const;
	bool getIBLEDiff() const;
	bool getIBLEDirect() const;
	bool getIBLESpec() const;
	const EmmiterUIData& getNewEmmiterUIData() const;

private:
	UI() = default;

private:
	void renderImgui();
	void renderLogImgui();
	void renderMenuImgui();
	void renderSceneSettingsImgui();
	void renderViewSettingsImgui();
	void renderScenePerModelsArrImgui(std::vector<graphics::MeshSystem::OpaqueShadingGroup::PerModel>& perModelsArr);
	void renderScenePerMeshesArrImgui(std::vector<graphics::MeshSystem::OpaqueShadingGroup::PerMesh>& perMeshesArr);
	void renderScenePerMaterialsArrImgui(std::vector<graphics::MeshSystem::OpaqueShadingGroup::PerMaterial>& perMaterialsArr);
	void renderSceneInstancesImgui();
	void renderPipelineShadersImgui(const graphics::PipelineShaders& pipelineShaders);
	void renderSceneLightSystemImgui();
	void renderPostProcessSettingsImgui();
	void renderPostProcessEV100SettingsImgui();
	void renderSceneParticleSystemImgui();
	void renderHelpImgui();

private:
	std::string to_string(const DirectX::XMVECTOR& v);
	std::string to_string(const DirectX::XMFLOAT4X4& f);
private:
	int32 viewSelectedInstanceId = 0;
	char sceneFilePath[256] = "../resources/scenes/scene1.json";
	bool detachFlashlightFromCamera = false;
	bool viewSceneShaders = true;
	bool viewNormalShaders = false;
	bool viewWireframeShaders = false;
	bool IBL_E_diff = true;
	bool IBL_E_direct = true;
	bool IBL_E_spec = true;
	int32 viewTextureFilter;
	int32 postprocessEv100Index = 0;
	EmmiterUIData newEmmiter;
	std::string logBuffer;
	std::queue<Event> eventsBuffer;
};