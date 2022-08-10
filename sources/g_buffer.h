#pragma once
#include "graphics.h"
class GBuffer
{
public:
	GBuffer(){}
	~GBuffer(){}

	//G-Buffer生成
	void create(ID3D11Device* device, DXGI_FORMAT format);

	
	//レンダーターゲットビュー
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	//シェーダーリソースビュー
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

	//シェーダーリソースビューのゲッター
	ID3D11ShaderResourceView* get_srv() const { return shader_resource_view.Get(); }
	//レンダーターゲットのゲッター（操作可能）
	ID3D11RenderTargetView* get_rtv()  { return render_target_view.Get(); }

};