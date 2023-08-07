#pragma once

#include <engine/math/RCamera.h>
#include <engine/graphics/Texture.h>
#include <engine/graphics/Particle.h>
#include <engine/utils/RandomGenerator.h>
#include <engine/utils/SolidVector.h>
#include <vector>
#include <algorithm>
#include <DirectXMath.h>
#include <memory>

namespace graphics
{

using EmmiterPosition = DirectX::XMVECTOR;

class Emmiter
{
public:
	enum EmitterUpdateStatus
	{
		PARTICLES_UPDATED,
		NONE,
	};
public:
	Emmiter(
		const utils::SolidVector<EmmiterPosition>::ID positionID, 
		const DirectX::XMVECTOR& spawnedParticleColor, 
		const DirectX::XMVECTOR& spawnParticleSize,
		const DirectX::XMVECTOR& spawnParticleSpeed,
		float32 spawnRatePerSecond, 
		float32 spawnCircleRadius,
		uint32 maxParticlesNum);
	EmitterUpdateStatus update(float64 elapsed, const math::RCamera& camera);
	void spawnNewParticle();
	void updateParticle(float64 elapsed);
	void removeOutlifedParticle();

	uint32 getParticlesNum() const;
	const utils::SolidVector<EmmiterPosition>::ID& getPositionID() const;
	const DirectX::XMVECTOR& getSpawnedParticleColor() const;
	const DirectX::XMVECTOR& getSpawnParticleSize() const;
	const DirectX::XMVECTOR& getSpawnParticleSpeed() const;
	float32 getSpawnRatePerSecond() const;
	float32 getSpawnCircleRadius() const;
	float64 getLastSpawnElapsed() const;
	uint32 getMaxParticlesNum() const;
	float64 getParticleLifetime() const;
	
private:
	DirectX::XMVECTOR randomPointInSphere(float32 sphereRadius);
private:
	std::vector<graphics::Particle> particles;
	utils::SolidVector<EmmiterPosition>::ID positionID;
	DirectX::XMVECTOR spawnedParticleColor;
	DirectX::XMVECTOR spawnParticleSize;
	DirectX::XMVECTOR spawnParticleSpeed;
	float32 spawnRatePerSecond;
	float32 spawnCircleRadius;
	float64 lastSpawnElapsed;
	uint32 maxParticlesNum;
	float64 particleLifetime;

	friend class ParticleSystem;
};

}