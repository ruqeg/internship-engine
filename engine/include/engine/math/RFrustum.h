#pragma once

#include <engine/math/RCamera.h>

#include <array>

namespace math
{

class RFrustum
{
public:
	void create(const RCamera& camera);

	const std::array<DirectX::XMVECTOR, 8>& getVertices() const;
	const DirectX::XMVECTOR& getNearVertical() const;
	const DirectX::XMVECTOR& getNearHorizontal() const;
	const DirectX::XMVECTOR& getNearLeftTop() const;

private:
	std::array<DirectX::XMVECTOR, 8> vertices;
	DirectX::XMVECTOR nearHorizontal;
	DirectX::XMVECTOR nearVertical;
};

}