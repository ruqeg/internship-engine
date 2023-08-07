#pragma once

#include <engine/graphics/Emmiter.h>
#include <engine/utils/TextureManager.h>
#include <engine/utils/ErrorLogger.h>
#include <engine/directx/D3D.h>
#include <engine/graphics/PipelineShaders.h>
#include <engine/utils/FileSystem.h>
#include <engine/utils/ErrorHandler.h>
#include <engine/graphics/VertexBuffer.h>
#include <engine/graphics/ConstantBuffer.h>
#include <engine/graphics/RingBuffer.hpp>
#include <engine/graphics/ComputeShader.h>
#include <vector>

namespace graphics
{

struct ParticleInstance
{
	DirectX::XMFLOAT4 tint_alpha;
	DirectX::XMFLOAT3 position;
	float32 rotationAngle;
	DirectX::XMFLOAT2 size;
	float32 time;
};

struct EmmiterCBS
{
	DirectX::XMUINT4 framesVertical;
	DirectX::XMUINT4 framesHorizontal;
	DirectX::XMFLOAT4 particleLifeTime;
};

struct ParticleGPUInstancec
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 force;
	DirectX::XMFLOAT3 color;
	float32 speed;
	float32 time;
};

struct ElapsedCBS
{
	float32 elapsed;
	DirectX::XMFLOAT3 _padding;
};

class ParticleSystem
{
public:
	enum ParticelSystemUpdateStatus
	{
		PARTICLES_UPDATED,
		NONE,
	};
public:
	static ParticleSystem& getInstance();
public:
	HRESULT initialize(UINT maxGPUParticlesNum);
	void render(UINT emmiterCBStartSlot, UINT RLUStartSlot, UINT DBFStartSlot, UINT EMVAStartSlot);
	HRESULT addSmokeEmitter(
		const utils::SolidVector<EmmiterPosition>::ID positionID,
		const DirectX::XMVECTOR& spawnedParticleColor,
		const DirectX::XMVECTOR& spawnParticleSize,
		const DirectX::XMVECTOR& spawnParticleSpeed,
		float32 spawnRatePerSecond,
		float32 spawnCircleRadius,
		uint32 maxParticlesNum);
	ParticelSystemUpdateStatus updateEmmiters(float64 elapsed, const math::RCamera& camera);
	void updateInstanceBuffer();
	void updateGPUParticles(float32 elapsed);
	const std::vector<Emmiter>& getEmmiters() const;
	graphics::RingBuffer<ParticleGPUInstancec>& getRingBuffer();
private:
	ParticleSystem() = default;
private:
	std::vector<Emmiter> m_emmiters;
	std::shared_ptr<graphics::Texture> m_smokeTextureRLU;
	std::shared_ptr<graphics::Texture> m_smokeTextureDBF;
	std::shared_ptr<graphics::Texture> m_smokeTextureEMVA;
	graphics::PipelineShaders m_cpuParticlePipelineShaders;
	graphics::PipelineShaders m_gpuParticlePipelineShaders;
	ComputeShader m_gpuParticlesPhisicsComputShader;
	ComputeShader m_rangeBufferUpdateComputShader;
	graphics::VertexBuffer<ParticleInstance> m_instanceBuffer;
	graphics::ConstantBuffer<EmmiterCBS> m_emmiterCB;
	graphics::RingBuffer<ParticleGPUInstancec> m_ringBuffer;
	graphics::ConstantBuffer<ElapsedCBS> m_elapsedCB;

private:
	static constexpr const wchar_t* SMOKE_TEXSTURE_RLU_PATH = L"../resources/textures/smoke_RLU.dds";
	static constexpr const wchar_t* SMOKE_TEXSTURE_DBF_PATH = L"../resources/textures/smoke_DBF.dds";
	static constexpr const wchar_t* SMOKE_TEXSTURE_EMVA_PATH = L"../resources/textures/smoke_MVEA.dds";
	static constexpr const wchar_t* GPU_SPARK_TEXTURE_PATH = L"../resources/textures/spark.dds";
	static constexpr const wchar_t* CPU_PARTICLE_VERTEX_SHADER_FILENAME = L"particle_vertex.cso";
	static constexpr const wchar_t* CPU_PARTICLE_PIXEL_SHADER_FILENAME = L"particle_pixel.cso";
	static constexpr const wchar_t* GPU_PARTICLE_VERTEX_SHADER_FILENAME = L"gpu_particle_vertex.cso";
	static constexpr const wchar_t* GPU_PARTICLE_PIXEL_SHADER_FILENAME = L"gpu_particle_pixel.cso";
	static constexpr const wchar_t* GPU_PARTICLE_PHISICS_COMPUTE_SHADER_FILENAME = L"gpu_particle_phisics_compute.cso";
	static constexpr const wchar_t* GPU_RANGE_BUFFER_UPDATE_COMPUTE_SHADER_FILENAME = L"range_buffer_update_compute.cso";
	static constexpr uint32 INSTANCE_BUFFER_START_SLOT = 5u;
	static constexpr uint32 TEXTURE_FRAMES_VERTICAL_NUM = 8u;
	static constexpr uint32 TEXTURE_FRAMES_HORIZONTAL_NUM = 8u;
};

}