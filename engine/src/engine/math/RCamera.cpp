#include <engine/math/RCamera.h>

namespace math
{

RCamera::RCamera()
{
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	positionVec = XMLoadFloat3(&position);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotationVec = XMLoadFloat3(&rotation);
	updateViewMatrix();
}

void RCamera::createProjectionMatrix(float32 fovRadians, float32 aspectRatio, float32 nearZ, float32 farZ)
{
	this->farZ = farZ;
	this->nearZ = nearZ;
	this->fov = fovRadians;
	projectionMat = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, farZ, nearZ);
	projectionInvMat = XMMatrixInverse(nullptr, projectionMat);
	XMStoreFloat4x4(&projection, projectionMat);
	XMStoreFloat4x4(&projectionInv, projectionInvMat);
}

const XMMATRIX& RCamera::getViewMatrix() const
{
	return viewMat;
}

const XMMATRIX& RCamera::getProjectionMatrix() const
{
	return projectionMat;
}

const XMMATRIX& RCamera::getViewInvMatrix() const
{
	return viewInvMat;
}

const XMMATRIX& RCamera::getProjectionInvMatrix() const
{
	return projectionInvMat;
}

const XMMATRIX& RCamera::getMatrix() const
{
	return mainMat;
}

const XMMATRIX& RCamera::getInvMatrix() const
{
	return mainInvMat;
}

const XMFLOAT4X4& RCamera::getViewFloat4x4() const
{
	return view;
}

const XMFLOAT4X4& RCamera::getProjectionFloat4x4() const
{
	return projection;
}

const XMFLOAT4X4& RCamera::getViewInvFloat4x4() const
{
	return viewInv;
}

const XMFLOAT4X4& RCamera::getProjectionInvFloat4x4() const
{
	return projectionInv;
}

const XMFLOAT4X4& RCamera::getFloat4x4() const
{
	return main;
}

const XMFLOAT4X4& RCamera::getInvFloat4x4() const
{
	return mainInv;
}

const XMVECTOR& RCamera::getPositionVector() const
{
	return positionVec;
}

const XMFLOAT3& RCamera::getPositionFloat3() const
{
	return position;
}

const XMVECTOR& RCamera::getRotationVector() const
{
	return rotationVec;
}

const XMFLOAT3& RCamera::getRotationFloat3() const
{
	return rotation;
}

const XMVECTOR& RCamera::getRightVector() const
{
	return rightVec;
}

const XMVECTOR& RCamera::getTopVector() const
{
	return topVec;
}

const XMVECTOR& RCamera::getForwardVector() const
{
	return forwardVec;
}

float32 RCamera::getFovRadians() const
{
	return fov;
}

float32 RCamera::getNearZ() const
{
	return nearZ;
}

float32 RCamera::getFarZ() const
{
	return farZ;
}

void RCamera::setPosition(const XMVECTOR& newPosition)
{
	XMStoreFloat3(&position, newPosition);
	positionVec = newPosition;
}

void RCamera::setPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	positionVec = XMLoadFloat3(&position);
}

void RCamera::addPosition(const XMVECTOR& newPosition)
{
	positionVec += newPosition;
	XMStoreFloat3(&position, positionVec);
}

void RCamera::addPosition(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	positionVec = XMLoadFloat3(&position);
}

void RCamera::setRotation(const XMVECTOR& newRotation)
{
	XMStoreFloat3(&rotation, newRotation);
	rotationVec = newRotation;
}

void RCamera::setRotation(float x, float y, float z)
{
	rotation = XMFLOAT3(x, y, z);
	rotationVec = XMLoadFloat3(&rotation);
}

void RCamera::addRotation(const XMVECTOR& newRotation)
{
	rotationVec += newRotation;
	XMStoreFloat3(&rotation, rotationVec);
}

void RCamera::addRotation(float x, float y, float z)
{
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;
	rotationVec = XMLoadFloat3(&rotation);
}

void RCamera::updateViewMatrix()
{
	XMMATRIX cameraRotationMat = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMVECTOR cameraTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, cameraRotationMat);
	cameraTarget += positionVec;
	XMVECTOR upDir = XMVector3TransformCoord(DEFAULT_TOP_VECTOR, cameraRotationMat);
	viewMat = XMMatrixLookAtLH(positionVec, cameraTarget, upDir);
	viewInvMat = XMMatrixInverse(nullptr, viewMat);
	XMStoreFloat4x4(&view, viewMat);
	XMStoreFloat4x4(&viewInv, viewInvMat);

	forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, cameraRotationMat);
	topVec = XMVector3TransformCoord(DEFAULT_TOP_VECTOR, cameraRotationMat);
	rightVec = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, cameraRotationMat);
}

void RCamera::updateMainMatrix()
{
	mainMat = XMMatrixMultiply(viewMat, projectionMat);
	mainInvMat = XMMatrixInverse(nullptr, mainMat);
	XMStoreFloat4x4(&main, mainMat);
	XMStoreFloat4x4(&mainInv, mainInvMat);
}

}