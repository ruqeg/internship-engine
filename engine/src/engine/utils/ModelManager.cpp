#include <engine/utils/ModelManager.h>

namespace utils
{

void ModelManager::initialize()
{
	//TODO
}

ModelManager& ModelManager::getInstance()
{
	static ModelManager instance;
	return instance;
}

std::optional<ModelManager::sharedModel_t> ModelManager::getModel(const std::string& filename)
{
	if (const auto search = map.find(filename); search != map.end())
	{
		return search->second;
	}
	else
	{
		std::optional<std::shared_ptr<graphics::Model>> model = loadModel(filename);
		if (model.has_value())
		{
			map[filename] = model.value();
		}
		return model;
	}
}

std::optional<ModelManager::sharedModel_t> ModelManager::loadModel(const std::string& filename)
{
	uint32 flags = uint32(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
	// aiProcess_Triangulate - ensure that all faces are triangles and not polygonals, otherwise triangulare them
	// aiProcess_GenBoundingBoxes - automatically compute bounding box, though we could do that manually
	// aiProcess_ConvertToLeftHanded - Assimp assumes left-handed basis orientation by default, convert for Direct3D
	// aiProcess_CalcTangentSpace - computes tangents and bitangents, they are used in advanced lighting

	Assimp::Importer importer;
	const aiScene* assimpScene = importer.ReadFile(filename, flags);
	if (assimpScene == NULL)
	{
		return std::nullopt;
	}

	uint32_t numMeshes = assimpScene->mNumMeshes;

	// Load vertex data
	graphics::Model model;
	model.name = filename;
	model.aabb = {};
	model.meshes.resize(numMeshes);
	model.ranges.resize(numMeshes);

	uint32 modelVertexBufferDataSize = 0;
	uint32 modelIndexBufferDataSize = 0;
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		auto& srcMesh = assimpScene->mMeshes[i];
		modelVertexBufferDataSize += srcMesh->mNumVertices;
		modelIndexBufferDataSize += 3 * srcMesh->mNumFaces;
	}
	std::vector<graphics::Mesh::Vertex> modelVertexBufferData(modelVertexBufferDataSize);
	std::vector<DWORD> modelIndexBufferData(modelIndexBufferDataSize);

	static_assert(sizeof(DirectX::XMFLOAT3) == sizeof(aiVector3D));
	static_assert(sizeof(math::IndexTriangle) == 3 * sizeof(uint32));
	
	uint32 gv = 0;
	uint32 gi = 0;
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		const uint32 vertexOffset = gv;
		const uint32 indexOffset = gi;

		auto& srcMesh = assimpScene->mMeshes[i];
		auto& dstMesh = model.meshes[i];

		dstMesh.name = srcMesh->mName.C_Str();
		dstMesh.aabb.min = XMVectorSet(srcMesh->mAABB.mMin.x, srcMesh->mAABB.mMin.y, srcMesh->mAABB.mMin.z, 1.0f);
		dstMesh.aabb.max = XMVectorSet(srcMesh->mAABB.mMax.x, srcMesh->mAABB.mMax.y, srcMesh->mAABB.mMax.z, 1.0f);

		dstMesh.vertices.resize(srcMesh->mNumVertices);
		dstMesh.triangles.resize(srcMesh->mNumFaces);

		for (uint32 v = 0; v < srcMesh->mNumVertices; ++v)
		{
			graphics::Mesh::Vertex& vertex = dstMesh.vertices[v];
			vertex.position = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mVertices[v]);
			vertex.textcoord = reinterpret_cast<DirectX::XMFLOAT2&>(srcMesh->mTextureCoords[0][v]);
			vertex.normal = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mNormals[v]);
			vertex.tangent = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mTangents[v]);
			vertex.bitangent = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mBitangents[v]);
			vertex.bitangent.x *= -1.0f;
			vertex.bitangent.y *= -1.0f;
			vertex.bitangent.z *= -1.0f;
			modelVertexBufferData[gv++] = vertex;
		}

		std::vector<math::RTriangle> faces(srcMesh->mNumFaces);

		for (uint32 f = 0; f < srcMesh->mNumFaces; ++f)
		{
			const auto& face = srcMesh->mFaces[f];
			assert(face.mNumIndices == 3);
			dstMesh.triangles[f] = *reinterpret_cast<math::IndexTriangle*>(face.mIndices);
			const uint32 i0 = dstMesh.triangles[f].i0;
			const uint32 i1 = dstMesh.triangles[f].i1;
			const uint32 i2 = dstMesh.triangles[f].i2;
			modelIndexBufferData[gi++] = i0;
			modelIndexBufferData[gi++] = i1;
			modelIndexBufferData[gi++] = i2;
			const auto& v0 = dstMesh.vertices[i0];
			const auto& v1 = dstMesh.vertices[i1];
			const auto& v2 = dstMesh.vertices[i2];
			faces[f] = math::RTriangle(XMLoadFloat3(&v0.position), XMLoadFloat3(&v1.position), XMLoadFloat3(&v2.position));
		}

		graphics::Model::MeshRange meshRange;
		uint32 s = gi - int64(indexOffset);
		meshRange.indexNum = gi - indexOffset;
		meshRange.indexOffset = indexOffset;
		meshRange.vertexNum = gv - vertexOffset;
		meshRange.vertexOffset = vertexOffset;
		model.ranges[i] = meshRange;

		dstMesh.trinagleOctree.build(faces, dstMesh.aabb);

		aiString texturePath;
		for (int j = 0; assimpScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, j, &texturePath) == aiReturn::aiReturn_SUCCESS; ++j)
		{
			dstMesh.texturesPath.push_back(texturePath.C_Str());
		}
	}

	// Recursively load mesh instances (meshToModel transformation matrices)
	std::function<void(aiNode*)> loadInstances;
	loadInstances = [&loadInstances, &model](aiNode* node)
	{
		const DirectX::XMFLOAT4X4 nodeToParent = reinterpret_cast<const DirectX::XMFLOAT4X4&>(node->mTransformation.Transpose());
		const DirectX::XMMATRIX nodeToParentMatrix = DirectX::XMLoadFloat4x4(&nodeToParent);
		const DirectX::XMMATRIX nodeToParentInverseMatrix = DirectX::XMMatrixInverse(nullptr, nodeToParentMatrix);
		DirectX::XMFLOAT4X4 parentToNode;
		XMStoreFloat4x4(&parentToNode, nodeToParentInverseMatrix);

		// The same node may contain multiple meshes in its space, referring to them by indices
		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			uint32 meshIndex = node->mMeshes[i];
			model.meshes[meshIndex].instances.push_back(nodeToParent);
			model.meshes[meshIndex].instancesInv.push_back(parentToNode);
		}

		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			loadInstances(node->mChildren[i]);
		}
	};
	loadInstances(assimpScene->mRootNode);

	//TODO
	for (uint32 i = 0; i < model.meshes.size(); ++i)
	{
		DirectX::XMStoreFloat4x4(&model.meshes[i].meshToModel, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&model.meshes[i].meshToModelInv, DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixIdentity()));
	}

	model.vertexBuffer.initialize(modelVertexBufferData.data(), modelVertexBufferData.size());
	model.indexBuffer.initialize(modelIndexBufferData.data(), modelIndexBufferData.size());
	
	for (auto& mesh : model.meshes)
	{
		model.aabb.max = DirectX::XMVectorMax(mesh.aabb.max, model.aabb.max);
		model.aabb.min = DirectX::XMVectorMin(mesh.aabb.min, model.aabb.min);
	}

	return std::make_shared<graphics::Model>(model);
}

}