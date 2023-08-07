#pragma once

#include <engine/graphics/Mesh.h>
#include <engine/graphics/VertexBuffer.h>
#include <engine/graphics/IndexBuffer.h>

#include <vector>
#include <string>

namespace graphics
{

struct Model
{
	struct MeshRange
	{
		uint32 vertexOffset;
		uint32 indexOffset;
		uint32 vertexNum;
		uint32 indexNum;
	};
	std::string name;
	math::RAABB aabb;
	std::vector<Mesh> meshes;
	std::vector<MeshRange> ranges;
	VertexBuffer<Mesh::Vertex> vertexBuffer;
	IndexBuffer indexBuffer;
};

}