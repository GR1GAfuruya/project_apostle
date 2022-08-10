#include "effekseer_effect_manager.h"
//������
void EffekseerEffectManager::initialize(Graphics& graphics)
{
	// �G�t�F�N�g�̃����_���[�̍쐬
	effekseer_renderer = EffekseerRendererDX11::Renderer::Create(graphics.get_device().Get(),
		graphics.get_dc().Get(), 2048);

	// �G�t�F�N�g�̃}�l�[�W���[�̍쐬
	effekseer_manager = Effekseer::Manager::Create(2048);

	// �`�惂�W���[���̐ݒ�
	effekseer_manager->SetSpriteRenderer(effekseer_renderer->CreateSpriteRenderer());
	effekseer_manager->SetRibbonRenderer(effekseer_renderer->CreateRibbonRenderer());
	effekseer_manager->SetRingRenderer(effekseer_renderer->CreateRingRenderer());
	effekseer_manager->SetTrackRenderer(effekseer_renderer->CreateTrackRenderer());
	effekseer_manager->SetModelRenderer(effekseer_renderer->CreateModelRenderer());

	// �e�N�X�`���A���f���A�J�[�u�A�}�e���A�����[�_�[�̐ݒ肷��B
	// ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
	effekseer_manager->SetTextureLoader(effekseer_renderer->CreateTextureLoader());
	effekseer_manager->SetModelLoader(effekseer_renderer->CreateModelLoader());
	effekseer_manager->SetMaterialLoader(effekseer_renderer->CreateMaterialLoader());
	effekseer_manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	//Effekseer��������W�n�Ōv�Z����
	effekseer_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffekseerEffectManager::finalize()
{
	if(effekseer_manager.Get() != NULL)
	{
		effekseer_manager.Reset();
	}

	if (effekseer_renderer.Get() != NULL)
	{
		effekseer_renderer.Reset();
	}
}

void EffekseerEffectManager::update(Graphics& graphics, float elapsed_time)
{
	effekseer_manager->Update(elapsed_time * 60.0f);
}

void EffekseerEffectManager::render(Camera& camera)
{
	//�r���[���v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
	effekseer_renderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera.get_view()));
	effekseer_renderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera.get_projection()));

	//�`��J�n
	effekseer_renderer->BeginRendering();

	//�`����s
	effekseer_manager->Draw();

	//�`��I��
	effekseer_renderer->EndRendering();
}
