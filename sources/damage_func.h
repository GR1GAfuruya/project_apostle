#pragma once
#include <functional>
#include "primitive.h"
#include <cereal/cereal.hpp>
#include "camera.h"
#include "game_pad.h"
struct AttackParam
{
	bool is_attack;//�U�������ǂ���
	int power;//�U����
	float invinsible_time;//�U���Ώۂɉۂ����G����
	Camera::CameraShakeParam camera_shake;//�J�����V�F�C�N
	Camera::HitStopParam hit_stop;//�q�b�g�X�g�b�v
	GamePad::Viberation hit_viberation;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("power", power),
			cereal::make_nvp("invinsible_time", invinsible_time),
			cereal::make_nvp("camera_shake", camera_shake),
			cereal::make_nvp("hit_stop", hit_stop),
			cereal::make_nvp("hit_viberation", hit_viberation)
		);
	}
};

//�_���[�W���󂯂����̂Ђ�ݕ�
enum class WINCE_TYPE
{
	NONE,//���݂Ȃ�
	SMALL,//�������Ђ��
	BIG,//�傫��������΂����
};
typedef std::function<bool(int, float, WINCE_TYPE)> AddDamageFunc;