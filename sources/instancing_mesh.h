#pragma once
#include "graphics.h"
class InstanceMesh
{
public:
	InstanceMesh(Graphics& graphics, const char* fbx_filename, const int max_instance);

    void active(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* alter_pixcel_shader);

    void render(Graphics& graphics);
    struct Instance
    {
        DirectX::XMFLOAT4 quaternion = {0,0,0,1};
        DirectX::XMFLOAT3 position{ 0,0,0 };
        DirectX::XMFLOAT3 scale{ 1,1,1 };
    };

    void set_position(DirectX::XMFLOAT3 p, int index) { CPU_instance_data[index].position = p; }
    void set_orientation(DirectX::XMFLOAT4 o, int index) { CPU_instance_data[index].quaternion = o; }
    void set_scale(DirectX::XMFLOAT3 s, int index) { CPU_instance_data[index].scale = s; }
    void set_scale(float s, int index) { CPU_instance_data[index].scale = { s,s,s }; }

    DirectX::XMFLOAT3 get_position(int index) { return CPU_instance_data[index].position; }
    DirectX::XMFLOAT4 get_orientation(int index) { return CPU_instance_data[index].quaternion; }
    DirectX::XMFLOAT3 get_scale(int index) { return CPU_instance_data[index].scale; }
private:
    void ReplaceBufferContents(Graphics& graphics, ID3D11Buffer* buffer, size_t bufferSize, const void* data);

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
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;

    struct aligned_deleter { void operator()(void* p) { _aligned_free(p); } };
    std::unique_ptr<Instance[]> CPU_instance_data;
    std::shared_ptr<ModelResource> model;
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resources;
    std::unique_ptr<Constants<OBJECT_CONSTANTS>> object_constants{};
    std::unique_ptr<Constants<MATERIAL_CONSTANTS>> material_constants{};
    int used_instance_count;

public:
    DirectX::XMFLOAT3 pos = {};//デバッグ用仮置き
    DirectX::XMFLOAT3 dir = {0,0,0};//デバッグ用仮置き
};
