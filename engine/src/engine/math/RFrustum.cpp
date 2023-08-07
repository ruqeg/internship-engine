#include <engine/math/RFrustum.h>

namespace math
{

void RFrustum::create(const RCamera& camera)
{
	// Frustum vertices in clipped sphere
	const std::array<DirectX::XMVECTOR, 8> clipedVertices =
	{
		DirectX::XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(-1.0f,  1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet( 1.0f,  1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet( 1.0f, -1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),
		DirectX::XMVectorSet(-1.0f,  1.0f, 0.0f, 1.0f),
		DirectX::XMVectorSet( 1.0f,  1.0f, 0.0f, 1.0f),
		DirectX::XMVectorSet( 1.0f, -1.0f, 0.0f, 1.0f)
	};

	// Trasform frustum cliped vertices to world space
	for (int i = 0; i < 8; i++)
	{
		DirectX::XMVECTOR worldVertices = DirectX::XMVector4Transform(clipedVertices[i], camera.getInvMatrix());
		vertices[i] = worldVertices / DirectX::XMVectorGetW(worldVertices);
	}
	
	// Calculate nearHorizontal, nearVertical
	const DirectX::XMVECTOR NearClipLeftBottomVertex = vertices[0];
	const DirectX::XMVECTOR NearClipLeftTopVertex = vertices[1];
	const DirectX::XMVECTOR NearClipRightTopVertex = vertices[2];
	nearHorizontal = NearClipRightTopVertex - NearClipLeftTopVertex;
	nearVertical = NearClipLeftTopVertex - NearClipLeftBottomVertex;
}

const std::array<DirectX::XMVECTOR, 8>& RFrustum::getVertices() const
{
	return vertices;
}

const DirectX::XMVECTOR& RFrustum::getNearVertical() const
{
	return nearVertical;
}

const DirectX::XMVECTOR& RFrustum::getNearHorizontal() const
{
	return nearHorizontal;
}

const DirectX::XMVECTOR& RFrustum::getNearLeftTop() const
{
	return vertices[1];
}

}