#include "fullscreen_quad.h"
#include "shader.h"
#include "misc.h"

FullscreenQuad::FullscreenQuad(ID3D11Device* device)
{
	create_vs_from_cso(device, "shaders/fullscreen_quad_vs.cso", embedded_vertex_shader.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);
	create_ps_from_cso(device, "shaders/fullscreen_quad_ps.cso", embedded_pixel_shader.ReleaseAndGetAddressOf());
}

void FullscreenQuad::blit(ID3D11DeviceContext* dc,
	ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader, ID3D11VertexShader* replaced_vertex_shader)
{
	ID3D11ShaderResourceView* cached_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	dc->PSGetShaderResources(start_slot, num_views, cached_shader_resource_views);

	dc->PSSetShaderResources(start_slot, num_views, shader_resource_views);

	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(NULL);

	replaced_vertex_shader ? dc->VSSetShader(replaced_vertex_shader, 0, 0) : dc->VSSetShader(embedded_vertex_shader.Get(), 0, 0);

	replaced_pixel_shader ? dc->PSSetShader(replaced_pixel_shader, 0, 0) : dc->PSSetShader(embedded_pixel_shader.Get(), 0, 0);
	dc->PSSetShaderResources(start_slot, num_views, shader_resource_views);

	dc->Draw(4, 0);

	dc->PSSetShaderResources(start_slot, num_views, cached_shader_resource_views);
	for (ID3D11ShaderResourceView* cached_shader_resource_view : cached_shader_resource_views)
	{
		if (cached_shader_resource_view) cached_shader_resource_view->Release();
	}
}
