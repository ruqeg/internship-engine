#include "MyEngine.h"
#include <iostream>

#include <engine/utils/Timer.h>

//#define __CREATE_JSON

void initConsole();
void printFps(float64 elapsed);

DirectX::XMVECTOR randomHemisphere(float& NdotV, float i, float N)
{
	const float GOLDEN_RATIO = (1.0 + sqrt(5.0)) / 2.0;
	float theta = 2.0 * DirectX::XM_PI * i / GOLDEN_RATIO;
	float phiCos = NdotV = 1.0 - (i + 0.5) / N;
	float phiSin = sqrt(1.0 - phiCos * phiCos);
	float thetaCos, thetaSin;
	DirectX::XMScalarSinCos(&thetaSin, &thetaCos, theta);
	return DirectX::XMVectorSet(thetaCos * phiSin, thetaSin * phiSin, phiCos, 0);
}

float integralCosineOverHemisphere()
{
	const int N = 1000;
	float integral = 0.0f;
	for (uint32_t i = 0; i < N; ++i) 
	{
		float NdotV;
		DirectX::XMVECTOR v = randomHemisphere(NdotV, i, N);
		integral += std::max(DirectX::XMVectorGetZ(v), 0.0f);
	}

	integral *= (2.0f * DirectX::XM_PI) / (float)N;
	return integral;
}

enum class FileFormat
{
	NONE,
	PNG,
	TGA,
	HDR,
	BC1_LINEAR = DXGI_FORMAT_BC1_UNORM,			// RGB, 1 bit Alpha
	BC1_SRGB = DXGI_FORMAT_BC1_UNORM_SRGB,		// RGB, 1-bit Alpha, SRGB
	BC3_LINEAR = DXGI_FORMAT_BC3_UNORM,			// RGBA
	BC3_SRGB = DXGI_FORMAT_BC3_UNORM_SRGB,		// RGBA, SRGB
	BC4_UNSIGNED = DXGI_FORMAT_BC4_UNORM,		// GRAY, unsigned
	BC4_SIGNED = DXGI_FORMAT_BC4_SNORM,			// GRAY, signed
	BC5_UNSIGNED = DXGI_FORMAT_BC5_UNORM,		// RG, unsigned
	BC5_SIGNED = DXGI_FORMAT_BC5_SNORM,			// RG, signed
	BC6_UNSIGNED = DXGI_FORMAT_BC6H_UF16,		// RGB HDR, unsigned
	BC6_SIGNED = DXGI_FORMAT_BC6H_SF16,			// RGB HDR, signed
	BC7_LINEAR = DXGI_FORMAT_BC7_UNORM,			// RGBA Advanced
	BC7_SRGB = DXGI_FORMAT_BC7_UNORM_SRGB,		// RGBA Advanced, SRGB
};

HRESULT saveToFile(DirectX::ScratchImage& image, const wchar_t* filename, FileFormat format, bool generateMips, bool isCompressed)
{
	HRESULT hr = S_OK;

	const DirectX::ScratchImage* imagePtr = &image;
	
	
	DirectX::ScratchImage compressed;
	if (isCompressed)
	{
		if (FileFormat::BC6_UNSIGNED <= format && format <= FileFormat::BC7_SRGB)
			hr = DirectX::Compress(d3d::device, imagePtr->GetImages(), imagePtr->GetImageCount(), imagePtr->GetMetadata(),
				DXGI_FORMAT(format), DirectX::TEX_COMPRESS_PARALLEL, 1.f, compressed);
		else
			hr = DirectX::Compress(imagePtr->GetImages(), imagePtr->GetImageCount(), imagePtr->GetMetadata(),
				DXGI_FORMAT(format), DirectX::TEX_COMPRESS_PARALLEL, 1.f, compressed);

		imagePtr = &compressed;
	}

	DirectX::SaveToDDSFile(imagePtr->GetImages(), imagePtr->GetImageCount(), imagePtr->GetMetadata(), DirectX::DDS_FLAGS(0), filename);

	return hr;
}


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
#if !defined(__CREATE_JSON)
	initConsole();

	std::cout << integralCosineOverHemisphere() << " " << DirectX::XM_PI << std::endl;

	utils::Timer timer(60);
	float64 elapsed;

	MyEngine engine;
	MyEngine::d3dinit();

/*	utils::ReflectionCapture::getInstance().initialize();
	DirectX::ScratchImage image;
	auto cubemapTexture = utils::TextureManager::getInstance().getTexture(L"../resources/textures/night_street.dds").value();
	utils::ReflectionCapture::getInstance().generateDiffuseIrradianceCubemap(cubemapTexture->resource, cubemapTexture->shaderResourceView, 8u, image);
	saveToFile(image, L"diffuse_irradiance_cubemap.dds", FileFormat::BC6_UNSIGNED, false, true);
	utils::ReflectionCapture::getInstance().generateSpecularIrradianceCubemap(cubemapTexture->resource, cubemapTexture->shaderResourceView, 1024u, image);
	saveToFile(image, L"specular_reflection_cubemap.dds", FileFormat::BC6_UNSIGNED, true, true);
	utils::ReflectionCapture::getInstance().generateSpecularReflectance2DTexture(2048u, image);
	saveToFile(image, L"specular_reflectance_texture2D.dds", FileFormat::BC5_UNSIGNED, false, true);
	*/
	engine.initialize(hInstance, "HW12", "HW12_CLASS", 1920, 1080);
	while (engine.processMessages())
	{
		if (timer.frameElapsed(elapsed))
		{
			printFps(elapsed);
			engine.update(elapsed);
			engine.renderFrame(elapsed);
		}
	}

	MyEngine::d3ddeinit();
#else
	//
#endif
	return 0;
}

void printFps(float64 elapsed)
{
	std::cout << "FPS: " << static_cast<int>(1.0 / elapsed) << std::endl;
}

void initConsole()
{
	AllocConsole();
	FILE* dummy;
	auto s = freopen_s(&dummy, "CONOUT$", "w", stdout);
}