#include <engine/graphics/ParticleSystem.h>

namespace graphics
{

ParticleSystem& ParticleSystem::getInstance()
{
	static ParticleSystem instance;
	return instance;
}

HRESULT ParticleSystem::initialize(UINT maxGPUParticlesNum)
{
	m_smokeTextureRLU = utils::TextureManager::getInstance().getTexture(SMOKE_TEXSTURE_RLU_PATH).value_or(nullptr);
	m_smokeTextureDBF = utils::TextureManager::getInstance().getTexture(SMOKE_TEXSTURE_DBF_PATH).value_or(nullptr);
	m_smokeTextureEMVA = utils::TextureManager::getInstance().getTexture(SMOKE_TEXSTURE_EMVA_PATH).value_or(nullptr);
	if (m_smokeTextureRLU == nullptr || m_smokeTextureDBF == nullptr || m_smokeTextureEMVA == nullptr)
	{
		utils::ErrorLogger::log(E_FAIL, L"Can't load smoke textures");
		return E_FAIL;
	}

	const auto& exeFolderPathW = utils::FileSystem::exeFolderPathW();
	m_cpuParticlePipelineShaders.vertexShader.initialize(exeFolderPathW + CPU_PARTICLE_VERTEX_SHADER_FILENAME, {
		{ "INSTANCECOLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_BUFFER_START_SLOT, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1u },
		{ "INSTANCEPOSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, INSTANCE_BUFFER_START_SLOT, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1u },
		{ "INSTANCEROTATIONANGLE", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, INSTANCE_BUFFER_START_SLOT, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1u },
		{ "INSTANCESIZE", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, INSTANCE_BUFFER_START_SLOT, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1u },
		{ "INSTANCETIME", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, INSTANCE_BUFFER_START_SLOT, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1u }
	});
	m_cpuParticlePipelineShaders.pixelShader.initialize(exeFolderPathW + CPU_PARTICLE_PIXEL_SHADER_FILENAME);

	m_gpuParticlePipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + GPU_PARTICLE_VERTEX_SHADER_FILENAME, {});
	m_gpuParticlePipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + GPU_PARTICLE_PIXEL_SHADER_FILENAME);
	m_gpuParticlesPhisicsComputShader.initialize(utils::FileSystem::exeFolderPathW() + GPU_PARTICLE_PHISICS_COMPUTE_SHADER_FILENAME);
	m_rangeBufferUpdateComputShader.initialize(utils::FileSystem::exeFolderPathW() + GPU_RANGE_BUFFER_UPDATE_COMPUTE_SHADER_FILENAME);

	m_ringBuffer.initialize(maxGPUParticlesNum);

	m_emmiterCB.initialize();

	m_elapsedCB.initialize();

	return S_OK;
}

void ParticleSystem::updateInstanceBuffer()
{
	uint32 instanceBufferDataSize = 0u;
	for (auto& emmiter : m_emmiters)
		instanceBufferDataSize += emmiter.particles.size();

	if (instanceBufferDataSize < 1)
		return;

	uint32 index = 0u;
	std::vector<ParticleInstance> instanceBufferData(instanceBufferDataSize);
	for (auto& emmiter : m_emmiters)
		for (auto& particle : emmiter.particles)
		{
			ParticleInstance instance;
			DirectX::XMStoreFloat3(&instance.position, particle.position);
			instance.rotationAngle = particle.rotationAngle;
			DirectX::XMStoreFloat2(&instance.size, particle.size);
			DirectX::XMStoreFloat4(&instance.tint_alpha, particle.tint_alpha);
			instance.time = particle.time;
			instanceBufferData[index++] = instance;
		}

	m_instanceBuffer.initialize(instanceBufferData.data(), instanceBufferDataSize);
}

void ParticleSystem::updateGPUParticles(float32 elapsed)
{
	m_elapsedCB.data.elapsed = elapsed;
	m_elapsedCB.applyChanges(0u, MapTarget::CS);

	d3d::devcon->CSSetUnorderedAccessViews(0u, 1u, m_ringBuffer.getParticleBufferUAV().GetAddressOf(), nullptr);
	d3d::devcon->CSSetUnorderedAccessViews(1u, 1u, m_ringBuffer.getRangeBufferUAV().GetAddressOf(), nullptr);

	m_gpuParticlesPhisicsComputShader.bind();
	d3d::devcon->Dispatch(1, 1, 1);

	m_rangeBufferUpdateComputShader.bind();
	d3d::devcon->Dispatch(1, 1, 1);

	d3d::devcon->CSSetShader(nullptr, nullptr, 0u);
	ID3D11UnorderedAccessView* nullarr[]{ nullptr };
	d3d::devcon->CSSetUnorderedAccessViews(0u, 1u, nullarr, nullptr);
	d3d::devcon->CSSetUnorderedAccessViews(1u, 1u, nullarr, nullptr);
}

ParticleSystem::ParticelSystemUpdateStatus ParticleSystem::updateEmmiters(float64 elapsed, const math::RCamera& camera)
{
	bool particlesSpawned = false;
	for (auto& emmiter : m_emmiters)
		particlesSpawned |= emmiter.update(elapsed, camera) == Emmiter::PARTICLES_UPDATED;
	return particlesSpawned ? PARTICLES_UPDATED : NONE;
}

HRESULT ParticleSystem::addSmokeEmitter(
	const utils::SolidVector<EmmiterPosition>::ID positionID,
	const DirectX::XMVECTOR& spawnedParticleColor,
	const DirectX::XMVECTOR& spawnParticleSize,
	const DirectX::XMVECTOR& spawnParticleSpeed,
	float32 spawnRatePerSecond,
	float32 spawnCircleRadius,
	uint32 maxParticlesNum)
{
	m_emmiters.push_back(Emmiter(positionID, spawnedParticleColor, spawnParticleSize, spawnParticleSpeed, spawnRatePerSecond, spawnCircleRadius, maxParticlesNum));
	return S_OK;
}

void ParticleSystem::render(UINT emmiterCBStartSlot, UINT RLUStartSlot, UINT DBFStartSlot, UINT EMVAStartSlot)
{
	d3d::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_gpuParticlePipelineShaders.bind();

	d3d::devcon->VSSetShaderResources(0u, 1u, m_ringBuffer.getParticleBufferSRV().GetAddressOf());
	d3d::devcon->VSSetShaderResources(1u, 1u, m_ringBuffer.getRangeBufferSRV().GetAddressOf());
	d3d::devcon->PSSetShaderResources(10u, 1u, utils::TextureManager::getInstance().getTexture(GPU_SPARK_TEXTURE_PATH).value()->shaderResourceView.GetAddressOf());
	d3d::devcon->DrawInstancedIndirect(m_ringBuffer.getRangeBuffer().Get(), graphics::RangeBufferCBS::ALIGNED_BYTE_OFFSET_FOR_ARGS);
	ID3D11ShaderResourceView* nullarr[] = { nullptr };
	d3d::devcon->VSSetShaderResources(0u, 1u, nullarr);

	m_instanceBuffer.bind(INSTANCE_BUFFER_START_SLOT, 1, 0);
	m_cpuParticlePipelineShaders.bind();

	d3d::devcon->PSSetShaderResources(RLUStartSlot, 1, m_smokeTextureRLU->shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(DBFStartSlot, 1, m_smokeTextureDBF->shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(EMVAStartSlot, 1, m_smokeTextureEMVA->shaderResourceView.GetAddressOf());

	uint32 renderedInstances = 0u;
	for (auto& emmiter : m_emmiters)
	{
		DirectX::XMStoreUInt4(&m_emmiterCB.data.framesHorizontal, DirectX::XMVectorReplicate(TEXTURE_FRAMES_HORIZONTAL_NUM));
		DirectX::XMStoreUInt4(&m_emmiterCB.data.framesVertical, DirectX::XMVectorReplicate(TEXTURE_FRAMES_VERTICAL_NUM));
		DirectX::XMStoreFloat4(&m_emmiterCB.data.particleLifeTime, DirectX::XMVectorReplicate(emmiter.particleLifetime));
		m_emmiterCB.applyChanges(emmiterCBStartSlot, MapTarget::PS);

		uint32 numInstances = emmiter.particles.size();
		d3d::devcon->DrawInstanced(4u, numInstances, 0u, renderedInstances);
		renderedInstances += numInstances;
	}
}

const std::vector<Emmiter>& ParticleSystem::getEmmiters() const
{
	return m_emmiters;
}

graphics::RingBuffer<ParticleGPUInstancec>& ParticleSystem::getRingBuffer()
{
	return m_ringBuffer;
}

}