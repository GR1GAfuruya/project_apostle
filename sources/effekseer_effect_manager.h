#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "graphics.h"
#include "camera.h"

//�G�t�F�N�g�}�l�[�W���[
class EffekseerEffectManager
{
private:
	EffekseerEffectManager(){}
	~EffekseerEffectManager(){}

public:
	static EffekseerEffectManager& Instance()
	{
		static EffekseerEffectManager instance;
		return instance;
	}

	//������
	void initialize(Graphics& graphics);
	//�I����
	void finalize();
	//�X�V����
	void update(Graphics& graphics, float elapsed_time);
	//�`�揈��
	void render(Camera& camera);
	
	//Effekseer�}�l�[�W���[�̎擾
	Effekseer::ManagerRef get_effekseer_manager() {return effekseer_manager;}
	

private:
	Effekseer::ManagerRef effekseer_manager = nullptr;
	EffekseerRenderer::RendererRef effekseer_renderer = nullptr;
};
