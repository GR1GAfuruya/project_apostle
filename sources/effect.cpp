#include "graphics.h"
#include "effect.h"
#include "effect_manager.h"
#include "user.h"
Effect::Effect(const char* filename)
{
	//Effekseerのリソースを読み込む
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

	//Effekseer::Managerを取得
	auto effekseer_manager = EffectManager::Instance().get_effekseer_manager();

	//Effekseerエフェクトを読み込み
	effekseer_effect = Effekseer::Effect::Create(effekseer_manager, (EFK_CHAR*)utf16Filename);
}

Effect::~Effect()
{
	//破棄処理
	if(effekseer_effect != nullptr)
	{
		effekseer_effect.Reset();
	}
}

void Effect::play(const DirectX::XMFLOAT3& position, float scale)
{
	auto effekseer_manager = EffectManager::Instance().get_effekseer_manager();
	effekseer_handle = effekseer_manager->Play(effekseer_effect, position.x, position.y, position.z);

	effekseer_manager->SetScale(effekseer_handle, scale, scale, scale);
	
}

void Effect::stop()
{
	auto effekseer_manager = EffectManager::Instance().get_effekseer_manager();
	effekseer_manager->StopEffect(effekseer_handle);
}

void Effect::set_position(const DirectX::XMFLOAT3& position)
{
	auto effekseer_manager = EffectManager::Instance().get_effekseer_manager();

	effekseer_manager->SetLocation(effekseer_handle, position.x, position.y, position.z);
}

void Effect::set_scale(const DirectX::XMFLOAT3& scale)
{
	auto effekseer_manager = EffectManager::Instance().get_effekseer_manager();
	effekseer_manager->SetScale(effekseer_handle, scale.x, scale.y, scale.z);
	
}

void Effect::set_angle(float angle)
{
	auto effekseer_manager = EffectManager::Instance().get_effekseer_manager();
	angle = DirectX::XMConvertToRadians(angle);
	effekseer_manager->SetRotation(effekseer_handle, angle, angle, angle);
}

void Effect::rotate_to_target(DirectX::XMFLOAT3 target, float turn_speed)
{
	using namespace DirectX;
	auto effekseer_manager = EffectManager::Instance().get_effekseer_manager();
	XMFLOAT3 pos = get_position();
	XMVECTOR to_vec = Math::calc_vector_AtoB(pos, target);
	to_vec = XMVector3Normalize(to_vec);
	//effekseer_manager->(effekseer_handle, angle, angle, angle);

}

DirectX::XMFLOAT3 Effect::get_position() const
{
	auto effekseer_manager = EffectManager::Instance().get_effekseer_manager();
	const DirectX::XMFLOAT3 pos = { effekseer_manager->GetLocation(effekseer_handle).X,
		effekseer_manager->GetLocation(effekseer_handle).Y,
		effekseer_manager->GetLocation(effekseer_handle).Z };
	return pos;
}
