#include <engine/math/RTransform.h>

namespace math
{

RTransform::RTransform()
{
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	positionVec = XMLoadFloat3(&position);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotationVec = XMLoadFloat3(&rotation);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	scaleVec = XMLoadFloat3(&rotation);
	updateMatrix();
}

void RTransform::setPosition(const XMVECTOR& newPosition)
{
	XMStoreFloat3(&position, newPosition);
	positionVec = newPosition;;
}

void RTransform::setPosition(float32 x, float32 y, float32 z)
{
	position = XMFLOAT3(x, y, z);
	positionVec = XMLoadFloat3(&position);
}


void RTransform::addPosition(const XMVECTOR& newPosition)
{
	positionVec += newPosition;
	XMStoreFloat3(&position, positionVec);
}

void RTransform::addPosition(float32 x, float32 y, float32 z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	positionVec = XMLoadFloat3(&position);
}

void RTransform::setRotation(const XMVECTOR& newRotation)
{
	XMStoreFloat3(&rotation, newRotation);
	rotationVec = newRotation;
}

void RTransform::setRotation(float32 x, float32 y, float32 z)
{
	rotation = XMFLOAT3(x, y, z);
	rotationVec = XMLoadFloat3(&rotation);
}

void RTransform::addRotation(const XMVECTOR& newRotation)
{
	rotationVec += newRotation;
	XMStoreFloat3(&rotation, rotationVec);
}

void RTransform::addRotation(float32 x, float32 y, float32 z)
{
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;
	rotationVec = XMLoadFloat3(&rotation);
}

void RTransform::setScale(const XMVECTOR& newScale)
{
	XMStoreFloat3(&scale, newScale);
	scaleVec = newScale;
}

void RTransform::setScale(float32 x, float32 y, float32 z)
{
	scale = XMFLOAT3(x, y, z);
	scaleVec = XMLoadFloat3(&scale);
}

void RTransform::addScale(const XMVECTOR& newScale)
{
	scaleVec += newScale;
	XMStoreFloat3(&scale, scaleVec);
}

void RTransform::addScale(float32 x, float32 y, float32 z)
{
	scale.x += x;
	scale.y += y;
	scale.z += z;
	scaleVec = XMLoadFloat3(&scale);
}

const XMVECTOR& RTransform::getPositionVector() const
{
	return positionVec;
}

const XMFLOAT3& RTransform::getPositionFloat3() const
{
	return position;
}
const XMVECTOR& RTransform::getRotationVector() const
{
	return rotationVec;
}
const XMFLOAT3& RTransform::getRotationFloat3() const
{
	return rotation;
}

const XMVECTOR& RTransform::getScaleVector() const
{
	return scaleVec;
}

const XMFLOAT3& RTransform::getScaleFloat3() const
{
	return scale;
}

const XMMATRIX& RTransform::getModelMatrix() const
{
	return modelMat;
}

const XMFLOAT4X4& RTransform::getModelFloat4x4() const
{
	return modelFloat4x4;
}

const XMMATRIX& RTransform::getModelInvMatrix() const
{
	return modelInvMat;
}

const XMFLOAT4X4& RTransform::getModelInvFloat4x4() const
{
	return modelInvFloat4x4;
}

void RTransform::updateMatrix()
{
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX tranlateMat = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);
	modelMat = XMMatrixMultiply(scaleMat, rotationMat);
	modelMat = XMMatrixMultiply(modelMat, tranlateMat);
	modelInvMat = XMMatrixInverse(nullptr, modelMat);
	XMStoreFloat4x4(&modelFloat4x4, modelMat);
	XMStoreFloat4x4(&modelInvFloat4x4, modelInvMat);
}

}