#pragma once
#include "graphics.h"
class InstanceMesh
{
public:
	InstanceMesh(Graphics& graphics, const char* fbx_filename, int max_instance);

    void update(Graphics& graphics, float elapsed_time);

    void render(Graphics& graphics/*, const DirectX::XMFLOAT4X4& world*/);
    void create_pixel_shader(ID3D11Device* device, const char* cso_name);
    void ReplaceBufferContents(Graphics& graphics, ID3D11Buffer* buffer, size_t bufferSize, const void* data);
    void register_shader_resource(ID3D11Device* device, const wchar_t* filename);
private:
    struct Instance
    {
        DirectX::XMFLOAT4 quaternion = {0,0,0,1};
        DirectX::XMFLOAT3 position{ 0,0,0 };
        DirectX::XMFLOAT3 scale{ 1,1,1 };
    };

    struct OBJECT_CONSTANTS
    {
        DirectX::XMFLOAT4X4 global_transform;
    };


    struct MATERIAL_CONSTANTS
    {
        DirectX::XMFLOAT2 texcoord_offset{ 0.0f, 0.0f };
        DirectX::XMFLOAT2 texcoord_scale{ 1.0f, 1.0f };
        float emissive_power = 1.0f;
        DirectX::XMFLOAT3 pad;
    };

    struct shader_resources
    {
        MATERIAL_CONSTANTS material_data;
        ID3D11ShaderResourceView* shader_resource_views[4];
    };
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   input_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        instance_data;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertex_shader;

    struct aligned_deleter { void operator()(void* p) { _aligned_free(p); } };
    std::unique_ptr<Instance[]> CPU_instance_data;
    std::shared_ptr<ModelResource> model;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resources;
    std::unique_ptr<Constants<OBJECT_CONSTANTS>> object_constants{};
    std::unique_ptr<Constants<MATERIAL_CONSTANTS>> material_constants{};
    int used_instance_count;

public:
    DirectX::XMFLOAT3 pos = {};//デバッグ用仮置き
    DirectX::XMFLOAT3 dir = {0,0,0};//デバッグ用仮置き
};
