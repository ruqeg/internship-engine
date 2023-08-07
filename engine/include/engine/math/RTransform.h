#pragma once

#include <engine/types.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace math
{

class RTransform
{
public:
	RTransform();
	void setPosition(const XMVECTOR& newPosition);
	void setPosition(float32 x, float32 y, float32 z);
	void addPosition(const XMVECTOR& newPosition);
	void addPosition(float32 x, float32 y, float32 z);
	void setRotation(const XMVECTOR& newRotation);
	void setRotation(float32 x, float32 y, float32 z);
	void addRotation(const XMVECTOR& newRotation);
	void addRotation(float32 x, float32 y, float32 z);
	void setScale(const XMVECTOR& newScale);
	void setScale(float32 x, float32 y, float32 z);
	void addScale(const XMVECTOR& newScale);
	void addScale(float32 x, float32 y, float32 z);

	const XMVECTOR& getPositionVector() const;
	const XMFLOAT3& getPositionFloat3() const;
	const XMVECTOR& getRotationVector() const;
	const XMFLOAT3& getRotationFloat3() const;
	const XMVECTOR& getScaleVector() const;
	const XMFLOAT3& getScaleFloat3() const;

	const XMMATRIX& getModelMatrix() const;
	const XMFLOAT4X4& getModelFloat4x4() const;
	const XMMATRIX& getModelInvMatrix() const;
	const XMFLOAT4X4& getModelInvFloat4x4() const;

	void updateMatrix();

private:
	XMVECTOR positionVec;
	XMVECTOR rotationVec;
	XMVECTOR scaleVec;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	XMMATRIX modelMat;
	XMMATRIX modelInvMat;
	XMFLOAT4X4 modelFloat4x4;
	XMFLOAT4X4 modelInvFloat4x4;
};

}