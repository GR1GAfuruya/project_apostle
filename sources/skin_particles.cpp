#include "skin_particles.h"

SkinParticles::SkinParticles(ID3D11Device* device, size_t max_particle_count)
{
	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(skin_particle) * max_particle_count);
	buffer_desc.StructureByteStride = sizeof(skin_particle);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&buffer_desc, NULL, husk_particle_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(particle) * max_particle_count);
	buffer_desc.StructureByteStride = sizeof(particle);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&buffer_desc, NULL, updated_particle_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * 1);
	buffer_desc.StructureByteStride = sizeof(uint32_t);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&buffer_desc, NULL, completed_particle_counter_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * 32);
	buffer_desc.StructureByteStride = sizeof(uint32_t);
	buffer_desc.Usage = D3D11_USAGE_STAGING;
	buffer_desc.BindFlags = 0;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	buffer_desc.MiscFlags = 0;
	hr = device->CreateBuffer(&buffer_desc, NULL, copied_structure_count_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
	shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shader_resource_view_desc.Buffer.ElementOffset = 0;
	shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
	hr = device->CreateShaderResourceView(updated_particle_buffer.Get(), &shader_resource_view_desc, updated_particle_buffer_srv.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc;
	unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unordered_access_view_desc.Buffer.FirstElement = 0;
	unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
	unordered_access_view_desc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(husk_particle_buffer.Get(), &unordered_access_view_desc, husk_particle_buffer_uav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unordered_access_view_desc.Buffer.FirstElement = 0;
	unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
	unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	hr = device->CreateUnorderedAccessView(husk_particle_buffer.Get(), &unordered_access_view_desc, husk_particle_append_buffer_uav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unordered_access_view_desc.Buffer.FirstElement = 0;
	unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
	unordered_access_view_desc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(updated_particle_buffer.Get(), &unordered_access_view_desc, updated_particle_buffer_uav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unordered_access_view_desc.Buffer.FirstElement = 0;
	unordered_access_view_desc.Buffer.NumElements = 1;
	unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	hr = device->CreateUnorderedAccessView(completed_particle_counter_buffer.Get(), &unordered_access_view_desc, completed_particle_counter_buffer_uav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = sizeof(particle_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	create_vs_from_cso(device, "husk_particles_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(device, "husk_particles_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	create_gs_from_cso(device, "husk_particles_gs.cso", geometry_shader.ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "husk_particles_cs.cso", compute_shader.ReleaseAndGetAddressOf());

	create_ps_from_cso(device, "accumulate_husk_particles_ps.cso", accumulate_husk_particles_ps.ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "husk_particles_copy_buffer_cs.cso", copy_buffer_cs.ReleaseAndGetAddressOf());
}


static UINT align(UINT num, UINT alignment)
{
	return (num + (alignment - 1)) & ~(alignment - 1);
}
void SkinParticles::integrate(ID3D11DeviceContext* immediate_context, float delta_time)
{
	HRESULT hr{ S_OK };

	_ASSERT_EXPR(particle_data.particle_count <= max_particle_count, L"");
	if (particle_data.particle_count == 0 || state >= 5)
	{
		return;
	}

	immediate_context->CSSetUnorderedAccessViews(0, 1, husk_particle_buffer_uav.GetAddressOf(), nullptr);
	immediate_context->CSSetUnorderedAccessViews(1, 1, updated_particle_buffer_uav.GetAddressOf(), nullptr);
	UINT initial_count{ transitioned_particle_count };
	immediate_context->CSSetUnorderedAccessViews(2, 1, completed_particle_counter_buffer_uav.GetAddressOf(), &initial_count);

	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	immediate_context->CSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());

	immediate_context->CSSetShader(compute_shader.Get(), NULL, 0);

	UINT num_threads = align(particle_data.particle_count, 256);
	immediate_context->Dispatch(num_threads / 256, 1, 1);

	ID3D11UnorderedAccessView* null_unordered_access_view{};
	immediate_context->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);
	immediate_context->CSSetUnorderedAccessViews(1, 1, &null_unordered_access_view, nullptr);
	immediate_context->CSSetUnorderedAccessViews(2, 1, &null_unordered_access_view, nullptr);

	immediate_context->CopyStructureCount(copied_structure_count_buffer.Get(), sizeof(uint32_t) * 1, completed_particle_counter_buffer_uav.Get());
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(copied_structure_count_buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	transitioned_particle_count = reinterpret_cast<uint32_t*>(mapped_subresource.pData)[1];
	immediate_context->Unmap(copied_structure_count_buffer.Get(), 0);

	state = transitioned_particle_count / particle_data.particle_count;
}

void SkinParticles::render(ID3D11DeviceContext* immediate_context)
{
	_ASSERT_EXPR(particle_data.particle_count <= max_particle_count, L"");
	if (particle_data.particle_count == 0)
	{
		return;
	}

	immediate_context->VSSetShader(vertex_shader.Get(), NULL, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), NULL, 0);
	immediate_context->GSSetShader(geometry_shader.Get(), NULL, 0);
	immediate_context->GSSetShaderResources(9, 1, updated_particle_buffer_srv.GetAddressOf());

	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	immediate_context->VSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());
	immediate_context->PSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());
	immediate_context->GSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());

	immediate_context->IASetInputLayout(NULL);
	immediate_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	immediate_context->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	immediate_context->Draw(static_cast<UINT>(particle_data.particle_count), 0);

	ID3D11ShaderResourceView* null_shader_resource_view{};
	immediate_context->GSSetShaderResources(9, 1, &null_shader_resource_view);
	immediate_context->VSSetShader(NULL, NULL, 0);
	immediate_context->PSSetShader(NULL, NULL, 0);
	immediate_context->GSSetShader(NULL, NULL, 0);
}

void SkinParticles::reset(ID3D11DeviceContext* immediate_context)
{
}

void SkinParticles::accumulate_husk_particles(ID3D11DeviceContext* immediate_context, std::function<void(ID3D11PixelShader*)> drawcallback)
{
}
