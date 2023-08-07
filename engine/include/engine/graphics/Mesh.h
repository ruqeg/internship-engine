#pragma once

#include <engine/math/RTransform.h>
#include <engine/math/IndexTriangle.h>
#include <engine/math/RAABB.h>
#include <engine/math/RTriangleOctree.h>

#include <string>
#include <vector>

namespace graphics
{

struct Mesh
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 textcoord;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 bitangent;
	};
	std::string name;
	math::RAABB aabb;
	std::vector<Vertex> vertices;
	std::vector<math::IndexTriangle> triangles;
	std::vector<DirectX::XMFLOAT4X4> instances;
	std::vector<DirectX::XMFLOAT4X4> instancesInv;
	std::vector<std::string> texturesPath;
	XMFLOAT4X4 meshToModelInv;
	XMFLOAT4X4 meshToModel;
	math::RTriangleOctree trinagleOctree;
};

}