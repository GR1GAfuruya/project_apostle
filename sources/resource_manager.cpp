#include "framework.h"
#include "resource_manager.h"

//---------------------------------//
//			���f�������̎d��        //
//--------------------------------//
//----��----//
/*�錾*/
// std::shared_ptr<SkinnedMesh> skinned_mesh; ���\�[�X�}�l�[�W���ɍ��킹�邽�߃V�F�A�[�h�|�C���^�ō쐬
/*����*/
//skinned_mesh = load_model_resource(�e�����);

std::shared_ptr<ModelResource> ResourceManager::load_model_resource(ID3D11Device* device, const char* fbx_filename, bool triangulate, float sampling_rate)
{
	// �}�b�v�̒����烂�f��������
	ModelMap::iterator iter = models.find(fbx_filename);
	if (iter != models.end())
	{
		//�����N���؂�Ă��Ȃ����m�F
		if (!iter->second.expired())  //second��map��[�l]�̕����̂���  *�L�[�Ȃ�first    expired :�����N���؂�Ă��Ȃ����̊m�F
		{
			//���f�����\�[�X��Ԃ�
			return iter->second.lock(); //lock: �Ď����Ă���shared_ptr�I�u�W�F�N�g���擾����B // https://cpprefjp.github.io/reference/memory/weak_ptr/lock.html
		}
	}
	// �����������Ă�������Ȃ����
	// �V�K���f�����\�[�X�ǂݍ���
	auto model = std::make_shared<ModelResource>(device, fbx_filename, triangulate, sampling_rate);
	// �}�b�v�ɓo�^
	models[fbx_filename] = model;
	return model;
}

//std::shared_ptr<Material> ResourceManager::load_material_resource(Graphics& graphics, string id)
//{
//	// �}�b�v�̒�����X�v���C�g������
//	MaterialMap::iterator iter = materials.find(id);
//	if (iter != materials.end())
//	{
//		//�����N���؂�Ă��Ȃ����m�F
//		if (!iter->second.expired())
//		{
//			return iter->second.lock();
//		}
//	}
//	// �����������Ă�������Ȃ����
//    // �V�K�X�v���C�g���\�[�X�ǂݍ���
//	auto material = std::make_shared<Material>(graphics, id);
//	// �}�b�v�ɓo�^
//	materials[id] = material;
//	return material;
//}