inline const glm::mat4x4& Camera::toMat() const
{
	return mainMat;
}

inline const glm::mat4x4& Camera::toMatInv() const
{
	return mainMatInv;
}

inline const glm::vec3& Camera::getPosition() const
{
	return position;
}

inline const glm::vec3& Camera::getRotation() const
{
	return angles;
}

inline glm::vec3 Camera::right() const
{
	return viewMatInv[0];
}

inline glm::vec3 Camera::top() const
{
	return viewMatInv[1];
}

inline glm::vec3 Camera::forward() const
{
	return viewMatInv[2];
}