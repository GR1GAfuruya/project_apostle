#include "gpu_particle.h"

#include <d3d11shader.h>

#include "imgui_include.h"
#include <random>
#include "shader.h"
#include "misc.h"
#include "texture.h"

GPU_Particles::GPU_Particles(ID3D11Device* device,const int max_particle)
{
	
	//���p�[�e�B�N���̐����X���b�h���Ŋ���؂�鐔�ɂ���
	particle_count = static_cast<size_t>((max_particle / THREAD_NUM_X)) * THREAD_NUM_X;


	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(PARTICLE_DATA) * particle_count);
	buffer_desc.StructureByteStride = sizeof(PARTICLE_DATA);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	//�p�[�e�B�N���̎��̗p�o�b�t�@�[
	hr = device->CreateBuffer(&buffer_desc, nullptr, particle_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * particle_count);
	buffer_desc.StructureByteStride = sizeof(UINT);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//�X�^�b�N�p�o�b�t�@�[
	hr = device->CreateBuffer(&buffer_desc, nullptr, particle_pool_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t));
	buffer_desc.StructureByteStride = sizeof(uint32_t);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	buffer_desc.MiscFlags = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, particle_count_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�A���I�[�_�[�A�N�Z�X�r���[����
	D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc{};
	unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unordered_access_view_desc.Buffer.FirstElement = 0;
	unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(particle_count);
	unordered_access_view_desc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(particle_buffer.Get(), &unordered_access_view_desc, particle_buffer_uav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	hr = device->CreateUnorderedAccessView(particle_pool_buffer.Get(), &unordered_access_view_desc, particle_pool_buffer_uav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	

	//�V�F�[�_�[���\�[�X�r���[����
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
	shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shader_resource_view_desc.Buffer.ElementOffset = 0;
	shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(particle_count);
	hr = device->CreateShaderResourceView(particle_buffer.Get(), &shader_resource_view_desc, particle_buffer_srv.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	//�萔�o�b�t�@
	particle_constants = std::make_unique<Constants<PARTICLE_CONSTANTS>>(device);


	create_vs_from_cso(device, "shaders/gpu_particles_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(device, "shaders/gpu_particles_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	create_gs_from_cso(device, "shaders/gpu_particles_gs.cso", geometry_shader.ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "shaders/gpu_particles_init_cs.cso", init_cs.ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "shaders/gpu_particles_emit_cs.cso", emit_cs.ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "shaders/gpu_particles_update_cs.cso", update_cs.ReleaseAndGetAddressOf());

	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, L"./resources/Effects/Textures/Particle04.png", texture.ReleaseAndGetAddressOf(), &texture2d_desc);
	particle_constants->data.emitter.emit_life = 0;
	//particle_constants->data.emitter.emit_life_time = 5;
	particle_constants->data.emitter.emit_rate = 16;
	particle_constants->data.emitter.emit_count = 0;
	particle_constants->data.particle_size = { 0.5f,0.5f };
	particle_constants->data.particle_count = static_cast<uint32_t>(particle_count);
}



void GPU_Particles::initialize(ID3D11DeviceContext* dc)
{
	
	//�p�[�e�B�N���̎������R���s���[�g�V�F�[�_�[�ɓ]��
	dc->CSSetShader(init_cs.Get(), NULL, 0);

	dc->CSSetUnorderedAccessViews(0, 1, particle_buffer_uav.GetAddressOf(), nullptr);
	dc->CSSetUnorderedAccessViews(1, 1, particle_pool_buffer_uav.GetAddressOf(), nullptr);
	//----<�R���s���[�g�V�F�[�_�[�̎��s>----//
	dc->Dispatch(particle_count /THREAD_NUM_X, 1, 1);
	
	//�K��null�ŃN���A
	ID3D11UnorderedAccessView* null_unordered_access_view{};
	dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);
	dc->CSSetUnorderedAccessViews(1, 1, &null_unordered_access_view, nullptr);
}

void GPU_Particles::launch_emitter(ID3D11DeviceContext* dc,float emit_life, Microsoft::WRL::ComPtr<ID3D11ComputeShader>  replace_emit_cs)
{
	if (replace_emit_cs)  emit_cs = replace_emit_cs;

	particle_constants->data.emitter.emit_life_time = emit_life;
}


//�p�[�e�B�N���̔���
void GPU_Particles::particle_emit(ID3D11DeviceContext* dc)
{
	//�萔�o�b�t�@���Z�b�g
	particle_constants->bind(dc, 9, CB_FLAG::CS_GS);
	//�p�[�e�B�N���̎������R���s���[�g�V�F�[�_�[�ɓ]��
	 dc->CSSetShader(emit_cs.Get(), NULL, 0);

	dc->CSSetUnorderedAccessViews(0, 1, particle_buffer_uav.GetAddressOf(), nullptr);
	dc->CSSetUnorderedAccessViews(1, 1, particle_pool_buffer_uav.GetAddressOf(), nullptr);
	
	//����������p�[�e�B�N���̐����X���b�h�̔{����
	UINT num_threads = (static_cast<UINT>(particle_constants->data.emitter.emit_rate) / THREAD_NUM_X) * THREAD_NUM_X;

	//�������悤�Ƃ��Ă��鐔���g�p�\�ȃp�[�e�B�N���̐��𒴂��Ă����甭�������Ȃ�
	if (get_particle_pool_count(dc) < num_threads)
	{
		return;
	}
	//----<�R���s���[�g�V�F�[�_�[�̎��s>----//
	dc->Dispatch(num_threads / THREAD_NUM_X, 1, 1);


	ID3D11UnorderedAccessView* null_unordered_access_view{};
	dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);
	dc->CSSetUnorderedAccessViews(1, 1, &null_unordered_access_view, nullptr);
}

void GPU_Particles::update(ID3D11DeviceContext* dc, float elapsed_time, ID3D11ComputeShader* replace_update_cs)
{
	//�G�~�b�^�[�̎������s����܂Ńp�[�e�B�N�����o
	if (particle_constants->data.emitter.emit_life_time > 0)
	{
		particle_emit(dc);
	};

	//�G�~�b�^�[�̍X�V
	particle_constants->data.emitter.emit_life_time -= elapsed_time;
	//�萔�o�b�t�@���Z�b�g
	particle_constants->bind(dc, 9, CB_FLAG::CS_GS);
	//�p�[�e�B�N���̎������R���s���[�g�V�F�[�_�[�ɓ]��
	replace_update_cs ? dc->CSSetShader(replace_update_cs, NULL, 0) : dc->CSSetShader(update_cs.Get(), NULL, 0);
	dc->CSSetUnorderedAccessViews(0, 1, particle_buffer_uav.GetAddressOf(), nullptr);
	dc->CSSetUnorderedAccessViews(1, 1, particle_pool_buffer_uav.GetAddressOf(), nullptr);
	//----<�R���s���[�g�V�F�[�_�[�̎��s>----//
	dc->Dispatch(particle_count / THREAD_NUM_X, 1, 1);


	ID3D11UnorderedAccessView* null_unordered_access_view{};
	dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);
	dc->CSSetUnorderedAccessViews(1, 1, &null_unordered_access_view, nullptr);
}

void GPU_Particles::render(ID3D11DeviceContext* dc, ID3D11Device* device)
{
	static DirectX::XMFLOAT3 ang = {};
#ifdef USE_IMGUI
	get_particle_pool_count(dc);
	ImGui::Begin("gpu_particle");
	ImGui::DragFloat3("angle", &ang.x);
	ImGui::DragFloat3("pos", &particle_constants->data.emitter.pos.x);
	ImGui::DragFloat2("scale", &particle_constants->data.particle_size.x,0.1);
	ImGui::DragFloat("rate", &particle_constants->data.emitter.emit_rate);
	int active_particle = static_cast<int>(particle_count) - particle_constants->data.particle_count;
	ImGui::DragInt("active_count", &active_particle);
	ImGui::DragInt("count", &particle_constants->data.particle_count);
	if(ImGui::Button("reset"))
	{
		initialize(dc);
	}
	ImGui::End();
#endif
	particle_constants->data.angle.x = DirectX::XMConvertToRadians(ang.x);
	particle_constants->data.angle.y = DirectX::XMConvertToRadians(ang.y);
	particle_constants->data.angle.z = DirectX::XMConvertToRadians(ang.z);
	const wchar_t* ps_hlsl = L"HLSL/gpu_particles_ps.hlsl";
	dc->VSSetShader(vertex_shader.Get(), NULL, 0);
	dc->PSSetShader(pixel_shader.Get(), NULL, 0);
	dc->GSSetShader(geometry_shader.Get(), NULL, 0);
	dc->GSSetShaderResources(9, 1, particle_buffer_srv.GetAddressOf());
	dc->PSSetShaderResources(5, 1, texture.GetAddressOf());
	
	dc->IASetInputLayout(NULL);
	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->Draw(static_cast<UINT>(particle_count), 0);

	compile_shader(device, ps_hlsl, pixel_shader.Get());

	

	ID3D11ShaderResourceView* null_shader_resource_view{};
	dc->GSSetShaderResources(9, 1, &null_shader_resource_view);
	dc->VSSetShader(NULL, NULL, 0);
	dc->PSSetShader(NULL, NULL, 0);
	dc->GSSetShader(NULL, NULL, 0);
	
}



UINT GPU_Particles::get_particle_pool_count(ID3D11DeviceContext* dc) const
{
	HRESULT hr{ S_OK };
	//pool�o�b�t�@�[��UAV���p�[�e�B�N���̃J�E���g�p�̃o�b�t�@�[�ɃR�s�[����
	dc->CopyStructureCount(particle_count_buffer.Get(), 0, particle_pool_buffer_uav.Get());
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	
	//�T�u���\�[�X����f�[�^��ǂݎ��
	hr = dc->Map(particle_count_buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	UINT count{ *static_cast<UINT*>(mapped_subresource.pData) };
	dc->Unmap(particle_count_buffer.Get(), 0);
	
	particle_constants->data.particle_count = count;
	return count;
}