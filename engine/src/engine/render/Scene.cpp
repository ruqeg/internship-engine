#include <engine/render/Scene.h>

namespace render
{

void Scene::parallelRender(const math::Camera& camera, const UnsafeOptBuffer& unsafeOptBuffer) const
{
	math::Frustum cameraFrustum;
	cameraFrustum.create(camera);

	const int32 minPerThread = 60000;
	const int64 bufferArea = unsafeOptBuffer.width * unsafeOptBuffer.height;
	const int32 maxThreads = (bufferArea + minPerThread - 1) / minPerThread;
	const int32 hardwareThreads = std::thread::hardware_concurrency();
	const int32 numThreads = std::min(hardwareThreads != 0 ? hardwareThreads : 2, maxThreads);
	const int64 blockSize = bufferArea / numThreads;

	std::vector<std::thread> threads(numThreads - 1);

	for (int32 i = 0; i < numThreads - 1; ++i)
	{
		threads[i] = std::thread(
			RenderBlock(&camera, &cameraFrustum, &mworld),
			unsafeOptBuffer,
			blockSize * i, blockSize * i + blockSize);
	}
	RenderBlock(&camera, &cameraFrustum, &mworld)(
		unsafeOptBuffer, blockSize * (numThreads - 1), bufferArea);
	std::for_each(threads.begin(), threads.end(),
		std::mem_fn(&std::thread::join));
}

}