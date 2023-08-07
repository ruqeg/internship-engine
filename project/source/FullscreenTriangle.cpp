#include "FullscreenTriangle.h"

void FullscreenTriangle::initialize()
{
	pipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + L"fullscreen_triangle_vertex.cso", {});
	pipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + L"fullscreen_triangle_pixel.cso");

	HRESULT hr = frustumConstanstBuffer.initialize();
	if (FAILED(hr))
	{
		throw std::exception("Failed to create frustumConstanstBuffer.");
	}
}

void FullscreenTriangle::setTexture(const std::shared_ptr<graphics::Texture>& cubeTexture)
{
	this->cubeTexture = cubeTexture;
}

void FullscreenTriangle::update(int32 shaderRegister, const math::RCamera& camera)
{
	// Frustum vertices in clipped sphere
	const std::array<DirectX::XMVECTOR, 3> clipedVertices =
	{
		DirectX::XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(-1.0f,  3.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet( 3.0f, -1.0f, 1.0f, 1.0f),
	};

	std::array<DirectX::XMVECTOR, 3> fullscrenTriangleNearPlane;

	// Trasform frustum cliped vertices to world space
	for (int i = 0; i < 3; i++)
	{
		DirectX::XMVECTOR worldVertices = DirectX::XMVector4Transform(clipedVertices[i], camera.getInvMatrix());
		fullscrenTriangleNearPlane[i] = worldVertices / DirectX::XMVectorGetW(worldVertices);
	}

	const DirectX::XMVECTOR NearClipLeftBottomVertex = fullscrenTriangleNearPlane[0];
	const DirectX::XMVECTOR NearClipLeftTopVertex = fullscrenTriangleNearPlane[1];
	const DirectX::XMVECTOR NearClipRightBottomVertex = fullscrenTriangleNearPlane[2];

	const DirectX::XMVECTOR NearClipLeftBottomVec = NearClipLeftBottomVertex - camera.getPositionVector();
	const DirectX::XMVECTOR NearClipLeftTopVec = NearClipLeftTopVertex - camera.getPositionVector();
	const DirectX::XMVECTOR NearClipRightBottomVec = NearClipRightBottomVertex - camera.getPositionVector();

	XMStoreFloat4(&frustumConstanstBuffer.data.nearLB_LT_RB[0], NearClipLeftBottomVec);
	XMStoreFloat4(&frustumConstanstBuffer.data.nearLB_LT_RB[1], NearClipLeftTopVec);
	XMStoreFloat4(&frustumConstanstBuffer.data.nearLB_LT_RB[2], NearClipRightBottomVec);
	frustumConstanstBuffer.applyChanges(shaderRegister, graphics::MapTarget::VS);
}

void FullscreenTriangle::render(UINT textureStartSlot)
{
	pipelineShaders.bind();

	d3d::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	d3d::devcon->PSSetShaderResources(textureStartSlot, 1, cubeTexture->shaderResourceView.GetAddressOf());

	d3d::devcon->Draw(3, 0);
}