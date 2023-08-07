#include <engine/directx/D3D.h>

namespace d3d
{

ID3D11Device5 * device = nullptr;
ID3D11DeviceContext4 * devcon = nullptr;
IDXGIFactory5 * factory = nullptr;

D3D& D3D::getInstance()
{
	static D3D instance;
	return instance;
}

void D3D::init(rateAdapterSuitabilityFunDecl_t rateAdapterSuitabilityFun)
{
	HRESULT hr;

	// Init D3D Factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(mfactory.GetAddressOf()));
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to create DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	hr = mfactory->QueryInterface(__uuidof(IDXGIFactory5), reinterpret_cast<void**>(mfactory5.GetAddressOf()));
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to Query IDXGIFactory5.");
		exit(-1);
	}

	//Init adapter
	std::vector<IDXGIAdapterData> IDXGIAdaptersDatas;

	IDXGIAdapterData currentIDXGIAdapterData;
	for (UINT index = 0; SUCCEEDED(mfactory->EnumAdapters(index, &currentIDXGIAdapterData.pIDXGIAdapter)); ++index)
	{
		HRESULT hr = currentIDXGIAdapterData.pIDXGIAdapter->GetDesc(&currentIDXGIAdapterData.description);
		if (FAILED(hr))
		{
			utils::ErrorLogger::log(hr, "Failed to Get Descrpition for unknow IDXGIApadter.");
		}
		IDXGIAdaptersDatas.push_back(currentIDXGIAdapterData);

		index += 1;
	}

	IDXGIAdapterData mostSuitabilityIDXGIAdapterData;
	uint32 maxAdapterRating = 0u;
	for (auto& IDXGIAdapterData : IDXGIAdaptersDatas)
	{
		uint32 adapterRating = rateAdapterSuitabilityFun(IDXGIAdapterData.pIDXGIAdapter, IDXGIAdapterData.description);
		if (adapterRating > maxAdapterRating) {
			mostSuitabilityIDXGIAdapterData = IDXGIAdapterData;
			maxAdapterRating = adapterRating;
		}
	}

	if (maxAdapterRating == 0u)
	{
		utils::ErrorLogger::log("Can't find suitability IDXGIAdapter.");
		exit(-1);
	}

	// Init D3D Device & Context
	const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_1;
	D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_1;
	hr = D3D11CreateDevice(
		mostSuitabilityIDXGIAdapterData.pIDXGIAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
		&featureLevelRequested,
		1,
		D3D11_SDK_VERSION,
		mdevice.GetAddressOf(),
		&featureLevelInitialized,
		mdevcon.GetAddressOf());

	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to D3D11CreateDevice.");
		exit(-1);
	}

	if (featureLevelRequested != featureLevelInitialized)
	{
		utils::ErrorLogger::log(hr, "D3D_FEATURE_LEVEL_11_0 != featureLevelInitialized.");
		exit(-1);
	}

	hr = mdevice->QueryInterface(__uuidof(ID3D11Device5), reinterpret_cast<void**>(mdevice5.GetAddressOf()));
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to Query ID3D11Device5.");
		exit(-1);
	}

	hr = mdevcon->QueryInterface(__uuidof(ID3D11DeviceContext4), reinterpret_cast<void**>(mdevcon4.GetAddressOf()));
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, "Failed to Query ID3D11DeviceContext4.");
		exit(-1);
	}

	// Write global pointers
	d3d::factory = mfactory5.Get();
	d3d::device = mdevice5.Get();
	d3d::devcon = mdevcon4.Get();
}

void D3D::deinit()
{
	d3d::factory = nullptr;
	d3d::device = nullptr;
	d3d::devcon = nullptr;
}

}