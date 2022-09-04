#pragma once
#include<d3d11.h>
#include <map>
#include <wrl.h>
#include "mesh_shader.h"
#include <mutex>
CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };

#define ST_SAMPLER Graphics::SAMPLER_STATE
#define ST_DEPTH Graphics::DEPTH_STENCIL_STATE
#define ST_BLEND Graphics::BLEND_STATE
#define ST_RASTERIZER Graphics::RASTERIZER
#define SHADER_TYPE Graphics::SHADER_TYPES
#define RENDER_TYPE MeshShader::RenderType
class Graphics
{
public:
	//------------<定数>-----------//
	//
	//サンプラーステート
	enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE, CLAMP };
	//Zステンシルステート
	enum class DEPTH_STENCIL_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF , DEPTH_STENCIL_COUNT};
	//ブレンドステート
	enum class BLEND_STATE
	{
		NO_PROCESS,
		ALPHA,
		ADD,
		SUBTRACTION,
		MULTIPLY,

		BLEND_STATE_COUNT,
	};
	//ラスタライザステート
	enum class RASTERIZER
	{
		SOLID_ONESIDE,                   // SOLID, 面カリングなし, 時計回りが表
		CULL_NONE,               // SOLID, 面カリングあり
		SOLID_COUNTERCLOCKWISE,  // SOLID, 面カリングなし, 反時計回りが表
		WIREFRAME_CULL_BACK,     // WIREFRAME, 後ろ向きの三角形を描画しない
		WIREFRAME_CULL_NONE,     // WIREFRAME, 常にすべての三角形を描画する

		RASTERIZER_COUNT,
	};
	//シェーダータイプ
	enum class SHADER_TYPES { LAMBERT, PBR };

	
	//------------<コンストラクタ/デストラクタ>-----------//
	Graphics(){}
	~Graphics();

	//------------<ゲッター/セッター>-----------//
	Microsoft::WRL::ComPtr<ID3D11Device> get_device() { return device; }							//DirectX11の機能にアクセスするためのデバイス。このデバイスから描画に必要なオブジェクトの生成などを行う		
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> get_dc() { return immediate_context; }				//描画コマンドの生成や発行を管理をする。D3D11CreateDeviceAndSwapChainで生成されるのはImmediate。Immediateではコマンドの生成からGPUへの発行まで行う。
	Microsoft::WRL::ComPtr<IDXGISwapChain> get_swap_chain() { return swap_chain; }						//レンダリング結果を出力するためのオブジェクト
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> get_render_target_view() { return render_target_view; }			//レンダーターゲットビューを出力結合ステージにバインドできる
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> get_depth_stencil_view() { return depth_stencil_view; }	//深度ステンシルビューインターフェイスは、深度ステンシルテスト中にテクスチャーリソースにアクセスする。
	Microsoft::WRL::ComPtr<ID3D11SamplerState> get_sampler_state(SAMPLER_STATE s)  { return sampler_states[static_cast<int>(s)]; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> get_depth_stencil_state(DEPTH_STENCIL_STATE d) { return depth_stencil_states[static_cast<int>(d)]; }	//深度ステンシルビューインターフェイスは、深度ステンシルテスト中にテクスチャーリソースにアクセスする。
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> get_rasterizer_state(RASTERIZER r) { return rasterizer_states[static_cast<int>(r)]; }
	Microsoft::WRL::ComPtr<ID3D11BlendState> get_blend_state(BLEND_STATE b) { return blend_states[static_cast<int>(b)]; }

	

	//------------<関数>-----------//
public:
	void initialize(HWND hwnd);
	//------------<変数>-----------//
private:
	//COMオブジェクト

	Microsoft::WRL::ComPtr<ID3D11Device> device;							//DirectX11の機能にアクセスするためのデバイス。このデバイスから描画に必要なオブジェクトの生成などを行う		
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;				//描画コマンドの生成や発行を管理をする。D3D11CreateDeviceAndSwapChainで生成されるのはImmediate。Immediateではコマンドの生成からGPUへの発行まで行う。
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;						//レンダリング結果を出力するためのオブジェクト
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;			//レンダーターゲットビューを出力結合ステージにバインドできる
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;		//深度ステンシルビューインターフェイスは、深度ステンシルテスト中にテクスチャーリソースにアクセスする。

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[5];

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::DEPTH_STENCIL_COUNT)];

	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[static_cast<int>(BLEND_STATE::BLEND_STATE_COUNT)];

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[static_cast<int>(RASTERIZER::RASTERIZER_COUNT)];
	//--maps--//
	std::map<SHADER_TYPES, std::shared_ptr<MeshShader>> shaders;
	//std::map<SHADER_TYPES, Shader* > shaders;
public:
	//Shader* shader = nullptr;
	std::shared_ptr<MeshShader> shader = nullptr;
	void set_depth_state(DEPTH_STENCIL_STATE z_stencil);
	void set_blend_state( BLEND_STATE blend);
	void set_rasterizer_state(RASTERIZER rasterizer);
	void set_graphic_state_priset( DEPTH_STENCIL_STATE z_stencil, BLEND_STATE blend, RASTERIZER rasterizer);
	void shader_activate(SHADER_TYPES sh, RENDER_TYPE rt);

	//ミューテックス取得
	std::mutex& get_mutex() { return mutex; }


private:
	std::mutex mutex;
};

