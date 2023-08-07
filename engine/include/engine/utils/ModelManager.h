#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <engine/graphics/Model.h>

#include <unordered_map>
#include <memory>
#include <optional>

namespace utils
{

class ModelManager
{
protected:
	using sharedModel_t = std::shared_ptr<graphics::Model>;
public:
	void initialize();
	std::optional<sharedModel_t> getModel(const std::string& filename);
	std::optional<sharedModel_t> loadModel(const std::string& filename);
public:
	static ModelManager& getInstance();
protected:
	ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;
protected:
	std::unordered_map<std::string, sharedModel_t> map;
	sharedModel_t unitShpere;
};

}