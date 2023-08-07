#pragma once

#include <engine/graphics/PixelShader.h>
#include <engine/graphics/HullShader.h>
#include <engine/graphics/DomainShader.h>
#include <engine/graphics/GeometryShader.h>
#include <engine/graphics/VertexShader.h>

namespace graphics
{

struct PipelineShaders
{
	VertexShader vertexShader;
	HullShader hullShader;
	DomainShader domainShader;
	GeometryShader geometryShader;
	PixelShader pixelShader;
	
	inline void bind()
	{
		vertexShader.bind();
		hullShader.bind();
		domainShader.bind();
		geometryShader.bind();
		pixelShader.bind();
	}
};

}