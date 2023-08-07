#pragma once

#include <engine/types.h>
#include <engine/utils/ErrorLogger.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>

#include <wrl/client.h>

#include <vector>
#include <functional>

namespace d3d
{

extern ID3D11Device5 * device;
extern ID3D11DeviceContext4 * devcon;
extern IDXGIFactory5 * factory;

struct IDXGIAdapterData
{
	IDXGIAdapter* pIDXGIAdapter;
	DXGI_ADAPTER_DESC description;
};

class D3D
{
private:
	using rateAdapterSuitabilityFunDecl_t = std::function<uint32(const IDXGIAdapter* pIDXGIAdapter, const DXGI_ADAPTER_DESC& description)>;
public:
	static D3D& getInstance();

public:
	void init(rateAdapterSuitabilityFunDecl_t rateAdapterSuitabilityFun);
	void deinit();

private:
	D3D() = default;
	D3D(D3D&) = delete;
	D3D(D3D&&) = delete;
	D3D& operator=(D3D&) = delete;
	D3D& operator=(D3D&&) = delete;

private:
	Microsoft::WRL::ComPtr<IDXGIFactory> mfactory;
	Microsoft::WRL::ComPtr<IDXGIFactory5> mfactory5;
	Microsoft::WRL::ComPtr<ID3D11Device> mdevice;
	Microsoft::WRL::ComPtr<ID3D11Device5> mdevice5;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mdevcon;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext4> mdevcon4;
	Microsoft::WRL::ComPtr<ID3D11Debug>	mdevdebug;
};

}