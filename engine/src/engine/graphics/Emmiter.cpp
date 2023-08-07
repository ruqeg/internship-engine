#include <engine/graphics/Emmiter.h>

namespace graphics
{

Emmiter::Emmiter(
	const utils::SolidVector<EmmiterPosition>::ID positionID,
	const DirectX::XMVECTOR& spawnedParticleColor,
	const DirectX::XMVECTOR& spawnParticleSize,
	const DirectX::XMVECTOR& spawnParticleSpeed,
	float32 spawnRatePerSecond,
	float32 spawnCircleRadius,
	uint32 maxParticlesNum)
	:
	positionID(positionID),
	spawnedParticleColor(spawnedParticleColor),
	spawnParticleSpeed(spawnParticleSpeed),
	spawnRatePerSecond(spawnRatePerSecond),
	spawnCircleRadius(spawnCircleRadius),
	spawnParticleSize(spawnParticleSize),
	maxParticlesNum(maxParticlesNum),
	particleLifetime(maxParticlesNum / spawnRatePerSecond),
	lastSpawnElapsed(0.0)
{}

Emmiter::EmitterUpdateStatus Emmiter::update(float64 elapsed, const math::RCamera& camera)
{
	lastSpawnElapsed += elapsed;
	if (lastSpawnElapsed > 1.0 / spawnRatePerSecond)
	{
		spawnNewParticle();
		lastSpawnElapsed = 0.0;
	}
	updateParticle(elapsed);
	removeOutlifedParticle();
	std::sort(particles.begin(), particles.end(), [camera](const Particle& p1, const Particle& p2) -> bool {
		const DirectX::XMVECTOR point1_cameraPos = DirectX::XMVectorSubtract(p1.position, camera.getPositionVector());
		const DirectX::XMVECTOR point2_cameraPos = DirectX::XMVectorSubtract(p2.position, camera.getPositionVector());
		const DirectX::XMVECTOR cameraDir = camera.getForwardVector();
		const float32 projDist1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(point1_cameraPos, cameraDir));
		const float32 projDist2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(point2_cameraPos, cameraDir));
		return projDist1 > projDist2;
	});
	return EmitterUpdateStatus::PARTICLES_UPDATED;
}

void Emmiter::spawnNewParticle()
{
	Particle newParticle;
	newParticle.constantSpeed = spawnParticleSpeed;
	newParticle.position = DirectX::XMVectorAdd(utils::SolidVector<graphics::EmmiterPosition>::getInstance()[positionID], randomPointInSphere(spawnCircleRadius));
	newParticle.rotationAngle = utils::RandomGenerator::getInstance().randomFloat(0, DirectX::XM_2PI);
	newParticle.size = spawnParticleSize;
	newParticle.tint_alpha = spawnedParticleColor;
	newParticle.time = 0.0;
	particles.push_back(newParticle);
}

void Emmiter::updateParticle(float64 elapsed)
{
	for (auto& particle : particles)
	{
		particle.time += elapsed;

		const DirectX::XMVECTOR newPosition = DirectX::XMVectorAdd(particle.position, DirectX::XMVectorScale(particle.constantSpeed, elapsed));
		const float32 newSizeScale = 1.f + elapsed / particleLifetime;
		
		const float TIME_CORNER_K = 1.f / 3.f;
		const float timerCorner = TIME_CORNER_K * particleLifetime;

		float32 newAlpha;
		if (particle.time < timerCorner)
			newAlpha = particle.time / timerCorner;
		else
			newAlpha = 1.0 - (particle.time - timerCorner) / (particleLifetime - timerCorner);

		particle.position = newPosition;
		particle.tint_alpha = DirectX::XMVectorSetW(particle.tint_alpha, newAlpha);
		particle.size = DirectX::XMVectorScale(particle.size, newSizeScale);
	}
}

void Emmiter::removeOutlifedParticle()
{
	for (int i = 0; i < particles.size(); ++i)
		if (particles[i].time > particleLifetime)
			particles.erase(particles.begin() + i);
}

DirectX::XMVECTOR Emmiter::randomPointInSphere(float32 sphereRadius)
{
	// !TODO upgrade code (without while)
	while (true)
	{
		const float32 x = utils::RandomGenerator::getInstance().randomFloat(-sphereRadius, sphereRadius);
		const float32 y = utils::RandomGenerator::getInstance().randomFloat(-sphereRadius, sphereRadius);
		const float32 z = utils::RandomGenerator::getInstance().randomFloat(-sphereRadius, sphereRadius);
		if (x * x + y * y + z * z <= sphereRadius * sphereRadius)
			return DirectX::XMVectorSet(x, y, z, 1.f);
	}
}

uint32 Emmiter::getParticlesNum() const
{
	return particles.size();
}

const utils::SolidVector<EmmiterPosition>::ID& Emmiter::getPositionID() const
{
	return positionID;
}

const DirectX::XMVECTOR& Emmiter::getSpawnedParticleColor() const
{
	return spawnedParticleColor;
}

const DirectX::XMVECTOR& Emmiter::getSpawnParticleSize() const
{
	return spawnParticleSize;
}

const DirectX::XMVECTOR& Emmiter::getSpawnParticleSpeed() const
{
	return spawnParticleSpeed;
}

float32 Emmiter::getSpawnRatePerSecond() const
{
	return spawnRatePerSecond;
}

float32 Emmiter::getSpawnCircleRadius() const
{
	return spawnCircleRadius;
}

float64 Emmiter::getLastSpawnElapsed() const
{
	return lastSpawnElapsed;
}

uint32 Emmiter::getMaxParticlesNum() const
{
	return maxParticlesNum;
}

float64 Emmiter::getParticleLifetime() const
{
	return particleLifetime;
}

}