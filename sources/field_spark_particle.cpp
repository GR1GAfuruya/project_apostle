#include "field_spark_particle.h"

#include <random>
#include "user.h"
#include "shader.h"
#include "misc.h"

field_spark_particles::field_spark_particles(ID3D11Device* device, DirectX::XMFLOAT3 initial_position)
{
	// Radius of outermost orbit 
	float outermost_radius{ 90 };
	// Interval between two particles
	float interval{ 8.0f };
	// Height of snowfall area
	float spark_area_height{ 60 };
	// Falling speed of snowflake
	float fall_speed{ -0.5f };

	// Orbit count
	int orbit_count = static_cast<int>(outermost_radius / interval);

	for (int orbit_index = 1; orbit_index <= orbit_count; ++orbit_index)
	{
		float radius = orbit_index * interval;

		for (float theta = 0; theta < 2 * 3.14159265358979f; theta += interval / radius)
		{
			for (float height = 0; height < spark_area_height; height += interval)
			{
				spark_particle_count++;
			}
		}
	}
	particles.resize(spark_particle_count);

	std::mt19937 mt{ std::random_device{}() };
	std::uniform_real_distribution<float> rand(-interval * 0.5f, +interval * 0.5f);

	size_t index{ 0 };
	for (int orbit_index = 1; orbit_index <= orbit_count; ++orbit_index)
	{
		float radius = orbit_index * interval;

		for (float theta = 0; theta < 2 * 3.14159265358979f; theta += interval / radius)
		{
			const float x{ radius * cosf(theta) };
			const float z{ radius * sinf(theta) };
			for (float height = -spark_area_height * 0.5f; height < spark_area_height * 0.5f; height += interval)
			{
				particles.at(index).position = { x + initial_position.x + rand(mt), height + initial_position.y + rand(mt), z + initial_position.z + rand(mt) };
				particles.at(index++).velocity = { fall_speed + rand(mt) * (fall_speed * 0.5f), fall_speed + rand(mt) * (fall_speed * 0.5f), fall_speed + rand(mt) * (fall_speed * 0.5f) };
			}
		}
	}

	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(spark_particle) * spark_particle_count);
	buffer_desc.StructureByteStride = sizeof(spark_particle);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = particles.data();
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, spark_particle_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc;
	unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unordered_access_view_desc.Buffer.FirstElement = 0;
	unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(spark_particle_count);
	unordered_access_view_desc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(spark_particle_buffer.Get(), &unordered_access_view_desc, spark_particle_buffer_uav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
	shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shader_resource_view_desc.Buffer.ElementOffset = 0;
	shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(spark_particle_count);
	hr = device->CreateShaderResourceView(spark_particle_buffer.Get(), &shader_resource_view_desc, spark_particle_buffer_srv.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	particle_constants = std::make_unique<Constants<PARTICLE_CONSTANTS>>(device);

	create_vs_from_cso(device, "shaders/gpu_particles_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(device, "shaders/field_spark_particles_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	create_gs_from_cso(device, "shaders/field_spark_particles_gs.cso", geometry_shader.ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "shaders/field_spark_particles_cs.cso", compute_shader.ReleaseAndGetAddressOf());

	particle_constants->data.current_eye_position = initial_position;
	particle_constants->data.previous_eye_position = initial_position;
	particle_constants->data.spark_area_height = spark_area_height;
	particle_constants->data.outermost_radius = outermost_radius;
	particle_constants->data.particle_size = 0.05f;
	particle_constants->data.particle_count = static_cast<uint32_t>(spark_particle_count);
}

static UINT align(UINT num, UINT alignment)
{
	return (num + (alignment - 1)) & ~(alignment - 1);
}
void field_spark_particles::update(ID3D11DeviceContext* dc, float elapsed_time, DirectX::XMFLOAT3 eye_position)
{
	HRESULT hr{ S_OK };

	particle_constants->data.previous_eye_position = particle_constants->data.current_eye_position;
	particle_constants->data.current_eye_position = eye_position;
	particle_constants->bind(dc, 9, CB_FLAG::CS_GS);
	dc->CSSetShader(compute_shader.Get(), NULL, 0);
	dc->CSSetUnorderedAccessViews(0, 1, spark_particle_buffer_uav.GetAddressOf(), nullptr);

	UINT num_threads = align(static_cast<UINT>(spark_particle_count), 256);
	dc->Dispatch(num_threads / 256, 1, 1);

	ID3D11UnorderedAccessView* null_unordered_access_view{};
	dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);

}

void field_spark_particles::render(ID3D11DeviceContext* dc)
{
	dc->VSSetShader(vertex_shader.Get(), NULL, 0);
	dc->PSSetShader(pixel_shader.Get(), NULL, 0);
	dc->GSSetShader(geometry_shader.Get(), NULL, 0);
	dc->GSSetShaderResources(9, 1, spark_particle_buffer_srv.GetAddressOf());
	particle_constants->bind(dc, 9, CB_FLAG::CS_GS);

	dc->IASetInputLayout(NULL);
	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->Draw(static_cast<UINT>(spark_particle_count), 0);

	ID3D11ShaderResourceView* null_shader_resource_view{};
	dc->GSSetShaderResources(9, 1, &null_shader_resource_view);
	dc->VSSetShader(NULL, NULL, 0);
	dc->PSSetShader(NULL, NULL, 0);
	dc->GSSetShader(NULL, NULL, 0);

#ifdef USE_IMGUI
	imgui_menu_bar("Effects", "gpu_particles", display_imgui);
	if (display_imgui)
	{
		int p_num = static_cast<uint32_t>(spark_particle_count);
		ImGui::Begin("field_spark");
		ImGui::DragInt("particlenum", &p_num);
		ImGui::End();
	}
#endif
}