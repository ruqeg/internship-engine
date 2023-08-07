#include <engine/graphics/DeferredShading.h>

namespace graphics
{

DeferredShading& DeferredShading::getInstance()
{
	static DeferredShading instance;
	return instance;
}

void DeferredShading::initialize(uint32 windowWidth, uint32 windowHeight)
{
	initializeBuffer(windowWidth, windowHeight, ALBEDO_BUFFER_FORMAT, gbuffer.albedoBuffer);
	initializeBuffer(windowWidth, windowHeight, ROUTHNESS_METALLNESS_BUFFER_FORMAT, gbuffer.roughnessMetallnessBuffer);
	initializeBuffer(windowWidth, windowHeight, NORMAL_BUFFER_FORMAT, gbuffer.normallBuffer);
	initializeBuffer(windowWidth, windowHeight, EMISSION_BUFFER_FORMAT, gbuffer.emissionBuffer);
	initializeBuffer(windowWidth, windowHeight, OBJECT_ID_BUFFER_FORMAT, gbuffer.objectIDBuffer);

	initializeDepthBuffer(windowWidth, windowHeight);
}

void DeferredShading::prerender(const std::array<float32, 4>& color)
{
	d3d::devcon->ClearRenderTargetView(gbuffer.albedoBuffer.renderTargetView.Get(), color.data());
	d3d::devcon->ClearRenderTargetView(gbuffer.emissionBuffer.renderTargetView.Get(), color.data());
	d3d::devcon->ClearRenderTargetView(gbuffer.normallBuffer.renderTargetView.Get(), color.data());
	d3d::devcon->ClearRenderTargetView(gbuffer.objectIDBuffer.renderTargetView.Get(), color.data());
	d3d::devcon->ClearRenderTargetView(gbuffer.roughnessMetallnessBuffer.renderTargetView.Get(), color.data());
	d3d::devcon->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
}

void DeferredShading::initializeBuffer(uint32 windowWidth, uint32 windowHeight, DXGI_FORMAT format, SRV_RTV_Textuer2D& buffer)
{
	D3D11_TEXTURE2D_DESC textureDesc{};

	textureDesc.Width              = windowWidth; // Width of the texture
	textureDesc.Height             = windowHeight; // Height of the texture
	textureDesc.MipLevels          = 1; // Number of mipmap levels, in this case, just 1
	textureDesc.ArraySize          = 1; // Number of textures in the array, in this case, just 1
	textureDesc.Format             = format; // Use DXGI_FORMAT_R8G8B8A8_UNORM for 8-bit per channel RGB texture
	textureDesc.SampleDesc.Count   = 1; // Number of multisamples per pixel
	textureDesc.SampleDesc.Quality = 0; // Image quality level
	textureDesc.Usage              = D3D11_USAGE_DEFAULT; // How the texture will be used
	textureDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // How the texture will be bound to the pipeline
	textureDesc.CPUAccessFlags     = 0; // No CPU access required
	textureDesc.MiscFlags          = 0; // No miscellaneous flags

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateTexture2D(&textureDesc, nullptr, buffer.texture2d.ReleaseAndGetAddressOf()),
		L"Can't initialize Texture2D for DefferedShading!");

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateShaderResourceView(buffer.texture2d.Get(), nullptr, buffer.shaderResourceView.ReleaseAndGetAddressOf()),
		L"Can't initialize SRV for DefferedShading!");

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateRenderTargetView(buffer.texture2d.Get(), nullptr, buffer.renderTargetView.ReleaseAndGetAddressOf()),
		L"Can't initialize RTV for DefferedShading!");
}

void DeferredShading::bind(const DeferredShadingBindTarget& bindTarget)
{
	d3d::devcon->PSSetShaderResources(bindTarget.albedoStartSlot, 1, gbuffer.albedoBuffer.shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(bindTarget.routhnessMetallnessStartSlot, 1, gbuffer.roughnessMetallnessBuffer.shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(bindTarget.normalStartSlot, 1, gbuffer.normallBuffer.shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(bindTarget.emissionStartSlot, 1, gbuffer.emissionBuffer.shaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(bindTarget.depthStartSlot, 1, depthShaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(bindTarget.objectIDStartSlot, 1, gbuffer.objectIDBuffer.shaderResourceView.GetAddressOf());
}

void DeferredShading::initializeDepthBuffer(uint32 windowWidth, uint32 windowHeight)
{
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = windowWidth;
	depthStencilDesc.Height = windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateTexture2D(&depthStencilDesc, NULL, depthStencilBuffer.GetAddressOf()),
		L"Failed to create depth stencil buffer");

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateDepthStencilView(depthStencilBuffer.Get(), &descDSV, depthStencilView.GetAddressOf()),
		L"Failed to create depth stencil view");

	D3D11_SHADER_RESOURCE_VIEW_DESC srv2Ddesc{};
	srv2Ddesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srv2Ddesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv2Ddesc.Texture2D.MipLevels = 1;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateShaderResourceView(depthStencilBuffer.Get(), &srv2Ddesc, depthShaderResourceView.GetAddressOf()),
		L"Failed to create depth stencil view");
}

Microsoft::WRL::ComPtr<ID3D11Texture2D>& DeferredShading::getDepthStencilBuffer()
{
	return depthStencilBuffer;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& DeferredShading::getDepthSRV()
{
	return depthShaderResourceView;
}

GBuffer& DeferredShading::getGBuffer()
{
	return gbuffer;
}

DeferredShadingBindTarget::DeferredShadingBindTarget(
	UINT albedoStartSlot,
	UINT routhnessMetallnessStartSlot,
	UINT normalStartSlot,
	UINT emissionStartSlot,
	UINT depthStartSlot,
	UINT objectIDStartSlot)
	:
	albedoStartSlot(albedoStartSlot),
	routhnessMetallnessStartSlot(routhnessMetallnessStartSlot),
	normalStartSlot(normalStartSlot),
	depthStartSlot(depthStartSlot),
	emissionStartSlot(emissionStartSlot),
	objectIDStartSlot(objectIDStartSlot)
{}

}