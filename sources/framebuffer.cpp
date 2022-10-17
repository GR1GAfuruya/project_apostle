#include "framebuffer.h"
#include "misc.h"


inline bool operator&(FB_FLAG lhs, FB_FLAG rhs)
{
	return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}

FrameBuffer::FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height, FB_FLAG flags)
{
	HRESULT hr{ S_OK };
	if (flags & FB_FLAG::COLOR)
	{
		
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = width;
		texture2d_desc.Height = height;
		texture2d_desc.MipLevels =  1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // DXGI_FORMAT_R8G8B8A8_UNORM
		texture2d_desc.SampleDesc.Count = 1; 
		texture2d_desc.SampleDesc.Quality = 0; 
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags =  0; 
		hr = device->CreateTexture2D(&texture2d_desc, 0, texture_2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
		render_target_view_desc.Format = texture2d_desc.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; 
		hr = device->CreateRenderTargetView(texture_2d.Get(), &render_target_view_desc, render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = texture2d_desc.Format;
		shader_resource_view_desc.ViewDimension =  D3D11_SRV_DIMENSION_TEXTURE2D; 
		shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
		shader_resource_view_desc.Texture2D.MipLevels =  1;
		hr = device->CreateShaderResourceView(texture_2d.Get(), &shader_resource_view_desc, shader_resource_views[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	if (flags & FB_FLAG::DEPTH_STENCIL)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = width;
		texture2d_desc.Height = height;
		texture2d_desc.MipLevels = 1; 
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = flags & FB_FLAG::STENCIL ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS; // DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_R16_TYPELESS
		texture2d_desc.SampleDesc.Count =  1; 
		texture2d_desc.SampleDesc.Quality =  0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = flags & FB_FLAG::STENCIL ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT; // DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT : DXGI_FORMAT_D16_UNORM
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; 
		depth_stencil_view_desc.Flags = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = flags & FB_FLAG::STENCIL ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R16_UNORM
		shader_resource_view_desc.ViewDimension =  D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
		shader_resource_view_desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc, shader_resource_views[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	tex_size[0] = width;
	tex_size[1] = height;

	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void FrameBuffer::clear(ID3D11DeviceContext* immediate_context,
	FB_FLAG flags, DirectX::XMFLOAT4 color, float depth, uint8_t stencil)
{
	if (flags & FB_FLAG::COLOR && render_target_view)
	{
		immediate_context->ClearRenderTargetView(render_target_view.Get(), reinterpret_cast<const FLOAT*>(&color));
	}
	if (flags & FB_FLAG::DEPTH_STENCIL && depth_stencil_view)
	{
		immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}
}

void FrameBuffer::activate(ID3D11DeviceContext* immediate_context, FB_FLAG flags)
{
	viewport_count = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediate_context->RSGetViewports(&viewport_count, cached_viewports);
	immediate_context->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> null_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> null_depth_stencil_view;
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMSetRenderTargets(1, flags & FB_FLAG::COLOR ? render_target_view.GetAddressOf() : null_render_target_view.GetAddressOf(), flags & FB_FLAG::DEPTH_STENCIL ? depth_stencil_view.Get() : null_depth_stencil_view.Get());
}

void FrameBuffer::deactivate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->RSSetViewports(viewport_count, cached_viewports);
	immediate_context->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(),
		cached_depth_stencil_view.Get());

}

