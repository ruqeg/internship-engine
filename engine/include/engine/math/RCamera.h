#pragma once

#include <engine/types.h>

#include <DirectXMath.h>

using namespace DirectX;

namespace math
{

class RCamera
{
public:
	RCamera();

	void createProjectionMatrix(float32 fovRadians, float32 aspectRatio, float32 nearZ, float32 farZ);

	const XMMATRIX& getViewMatrix() const;
	const XMMATRIX& getProjectionMatrix() const;
	const XMMATRIX& getViewInvMatrix() const;
	const XMMATRIX& getProjectionInvMatrix() const;
	const XMMATRIX& getMatrix() const;
	const XMMATRIX& getInvMatrix() const;
	const XMFLOAT4X4& getViewFloat4x4() const;
	const XMFLOAT4X4& getProjectionFloat4x4() const;
	const XMFLOAT4X4& getViewInvFloat4x4() const;
	const XMFLOAT4X4& getProjectionInvFloat4x4() const;
	const XMFLOAT4X4& getFloat4x4() const;
	const XMFLOAT4X4& getInvFloat4x4() const;
	
	const XMVECTOR& getPositionVector() const;
	const XMFLOAT3& getPositionFloat3() const;
	const XMVECTOR& getRotationVector() const;
	const XMFLOAT3& getRotationFloat3() const;

	const XMVECTOR& getRightVector() const;
	const XMVECTOR& getTopVector() const;
	const XMVECTOR& getForwardVector() const;

	float32 getFovRadians() const;
	float32 getNearZ() const;
	float32 getFarZ() const;

	void setPosition(const XMVECTOR& newPosition);
	void setPosition(float x, float y, float z);
	void addPosition(const XMVECTOR& newPosition);
	void addPosition(float x, float y, float z);
	void setRotation(const XMVECTOR& newRotation);
	void setRotation(float x, float y, float z);
	void addRotation(const XMVECTOR& newRotationo);
	void addRotation(float x, float y, float z);

	void updateViewMatrix();
	void updateMainMatrix();

private:
	XMVECTOR positionVec;
	XMVECTOR rotationVec;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMMATRIX viewMat;
	XMMATRIX viewInvMat;
	XMMATRIX projectionMat;
	XMMATRIX projectionInvMat;
	XMMATRIX mainMat;
	XMMATRIX mainInvMat;
	XMVECTOR forwardVec;
	XMVECTOR rightVec;
	XMVECTOR topVec;
	XMFLOAT4X4 view;
	XMFLOAT4X4 viewInv;
	XMFLOAT4X4 projection;
	XMFLOAT4X4 projectionInv;
	XMFLOAT4X4 main;
	XMFLOAT4X4 mainInv;

	float32 fov;
	float32 nearZ;
	float32 farZ;

private:
	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_TOP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
};

}