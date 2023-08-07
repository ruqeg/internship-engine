#pragma once

#include <engine/graphics/MeshSystem.h>
#include <engine/utils/ModelManager.h>
#include <engine/utils/TextureManager.h>
#include <engine/utils/FileSystem.h>
#include <engine/math/RCamera.h>
#include <engine/utils/SolidVector.h>
#include <engine/graphics/PostProcess.h>

#include "LightDebugger.h"
#include "FullscreenTriangle.h"

#include <nlohmann/json.hpp>

#include <fstream>

class SceneLoader
{
public:
	static SceneLoader& getInstance();
public:
	nlohmann::json createJsonFromFile(const std::string& path);
	void loadBackroundColorFromJson(const nlohmann::json& j, std::array<float32, 4>& bgcolor);
	void loadCameraFromJson(const nlohmann::json& j, float32 ratio, math::RCamera& camera);
	void loadFullscreenTriangleFromJson(const nlohmann::json& j, FullscreenTriangle& fullscrenTriangle);
	void loadMeshSystemFromJson(const nlohmann::json& j);
	void loadLightDebuggerFromJson(const nlohmann::json& j);
	void loadLightSystemFromJson(const nlohmann::json& j);
	void loadPostProccesFromJson(const nlohmann::json& j);
	std::optional<std::shared_ptr<graphics::Texture>> loadTextureFromJson(const nlohmann::json& j, const std::string& textureType);
private:
	SceneLoader() = default;
};