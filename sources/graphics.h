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
	//------------<�萔>-----------//
	//
	//�T���v���[�X�e�[�g
	enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE, CLAMP };
	//Z�X�e���V���X�e�[�g
	enum class DEPTH_STENCIL_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF , DEPTH_STENCIL_COUNT};
	//�u�����h�X�e�[�g
	enum class BLEND_STATE
	{
		NO_PROCESS,
		ALPHA,
		ADD,
		SUBTRACTION,
		MULTIPLY,

		BLEND_STATE_COUNT,
	};
	//���X�^���C�U�X�e�[�g
	enum class RASTERIZER
	{
		SOLID_ONESIDE,                   // SOLID, �ʃJ�����O�Ȃ�, ���v��肪�\
		CULL_NONE,               // SOLID, �ʃJ�����O����
		SOLID_COUNTERCLOCKWISE,  // SOLID, �ʃJ�����O�Ȃ�, �����v��肪�\
		WIREFRAME_CULL_BACK,     // WIREFRAME, �������̎O�p�`��`�悵�Ȃ�
		WIREFRAME_CULL_NONE,     // WIREFRAME, ��ɂ��ׂĂ̎O�p�`��`�悷��

		RASTERIZER_COUNT,
	};
	//�V�F�[�_�[�^�C�v
	enum class SHADER_TYPES { LAMBERT, PBR };

	
	//------------<�R���X�g���N�^/�f�X�g���N�^>-----------//
	Graphics(){}
	~Graphics();

	//------------<�Q�b�^�[/�Z�b�^�[>-----------//
	Microsoft::WRL::ComPtr<ID3D11Device> get_device() { return device; }							//DirectX11�̋@�\�ɃA�N�Z�X���邽�߂̃f�o�C�X�B���̃f�o�C�X����`��ɕK�v�ȃI�u�W�F�N�g�̐����Ȃǂ��s��		
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> get_dc() { return immediate_context; }				//�`��R�}���h�̐����┭�s���Ǘ�������BD3D11CreateDeviceAndSwapChain�Ő��������̂�Immediate�BImmediate�ł̓R�}���h�̐�������GPU�ւ̔��s�܂ōs���B
	Microsoft::WRL::ComPtr<IDXGISwapChain> get_swap_chain() { return swap_chain; }						//�����_�����O���ʂ��o�͂��邽�߂̃I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> get_render_target_view() { return render_target_view; }			//�����_�[�^�[�Q�b�g�r���[���o�͌����X�e�[�W�Ƀo�C���h�ł���
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> get_depth_stencil_view() { return depth_stencil_view; }	//�[�x�X�e���V���r���[�C���^�[�t�F�C�X�́A�[�x�X�e���V���e�X�g���Ƀe�N�X�`���[���\�[�X�ɃA�N�Z�X����B
	Microsoft::WRL::ComPtr<ID3D11SamplerState> get_sampler_state(SAMPLER_STATE s)  { return sampler_states[static_cast<int>(s)]; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> get_depth_stencil_state(DEPTH_STENCIL_STATE d) { return depth_stencil_states[static_cast<int>(d)]; }	//�[�x�X�e���V���r���[�C���^�[�t�F�C�X�́A�[�x�X�e���V���e�X�g���Ƀe�N�X�`���[���\�[�X�ɃA�N�Z�X����B
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> get_rasterizer_state(RASTERIZER r) { return rasterizer_states[static_cast<int>(r)]; }
	Microsoft::WRL::ComPtr<ID3D11BlendState> get_blend_state(BLEND_STATE b) { return blend_states[static_cast<int>(b)]; }

	

	//------------<�֐�>-----------//
public:
	void initialize(HWND hwnd);
	//------------<�ϐ�>-----------//
private:
	//COM�I�u�W�F�N�g

	Microsoft::WRL::ComPtr<ID3D11Device> device;							//DirectX11�̋@�\�ɃA�N�Z�X���邽�߂̃f�o�C�X�B���̃f�o�C�X����`��ɕK�v�ȃI�u�W�F�N�g�̐����Ȃǂ��s��		
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;				//�`��R�}���h�̐����┭�s���Ǘ�������BD3D11CreateDeviceAndSwapChain�Ő��������̂�Immediate�BImmediate�ł̓R�}���h�̐�������GPU�ւ̔��s�܂ōs���B
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;						//�����_�����O���ʂ��o�͂��邽�߂̃I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;			//�����_�[�^�[�Q�b�g�r���[���o�͌����X�e�[�W�Ƀo�C���h�ł���
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;		//�[�x�X�e���V���r���[�C���^�[�t�F�C�X�́A�[�x�X�e���V���e�X�g���Ƀe�N�X�`���[���\�[�X�ɃA�N�Z�X����B

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

	//�~���[�e�b�N�X�擾
	std::mutex& get_mutex() { return mutex; }


private:
	std::mutex mutex;
};

