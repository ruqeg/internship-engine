inline const glm::vec3& Transform::getPosition() const
{
	return position;
}

inline const glm::vec3& Transform::getRotation() const
{
	return rotation;
}

inline const glm::vec3& Transform::getScale() const
{
	return scale;
}

inline const glm::mat4x4& Transform::getRotationMat() const
{
	return rotationMat;
}

inline const glm::mat4x4& Transform::getRotationMatInv() const
{
	return rotationMatInv;
}

inline const glm::mat4x4& Transform::toMat() const
{
	return modelMat;
}

inline const glm::mat4x4& Transform::toMatInv() const
{
	return modelMatInv;
}
