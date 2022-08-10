#pragma once

#include "camera.h"
#include "player_move.h"
#include "skeletal_mesh.h"
#include "aura.h"
#include "gpu_particle.h"
//#include	"Effect.h"
//プレイヤー :final このクラスの継承ができないことを明示する
class Player final :public PlayerMove
{
public:
	Player(Graphics& graphics, Camera* camera);
	~Player()override;

	//初期化処理
	void initialize();
	//更新処理
	void update(Graphics& graphics, float elapsed_time, Camera* camera,Stage* stage);
	//描画処理
	//ディファードでレンダリングするオブジェクト
	void render_d(Graphics& graphics, float elapsed_time, Camera* camera);
	//フォワードレンダリングするオブジェクト
	void render_f(Graphics& graphics, float elapsed_time, Camera* camera);
	//デバッグ用GUI描画
	void debug_gui();

	

/////////////アニメーション////////////////
//アニメーション
	enum  PlayerAnimation
	{
		PLAYER_IDLE = 12,
		PLAYER_WALK,
		PLAYER_JUMP,
		PLAYER_FALL,
		PLAYER_LANDING,
	};

	enum class State
	{
		IDLE,
		MOVE,
		JUMP,
		FALL,
		LANDING,
	};
	DirectX::XMFLOAT3 get_camera_target_pos() { return DirectX::XMFLOAT3(position.x, position.y + height, position.z); }
private:
	//////遷移
	void transition_idle_state();
	void transition_attack_state();
	//void transition_move_state();
	//void transition_jump_state();
	//void transition_fall_state();
	//void transition_landing_state();


	//////アニメーションアップデート
	void update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera);
	void update_attack_state(Graphics& graphics, float elapsed_time, Camera* camera);
	//void update_move_state(float elapsed_time, Camera* camera);
	//void update_jump_state(float elapsed_time, Camera* camera);
	//void update_fall_state(float elapsed_time, Camera* camera);
	//void update_landing_state(float elapsed_time, Camera* camera);

	typedef void (Player::* p_Update)(Graphics& graphics,float elapsed_time, Camera* camera);
	p_Update p_update = &Player::update_idle_state;

	void Attack(Graphics& graphics, float elapsed_time);
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;
		
	std::unique_ptr<Aura> aura = nullptr;

	State state;

	//プレイヤーの移動入力処理
	bool input_move(float elapsedTime, Camera* camera);
	const DirectX::XMFLOAT3 get_move_vec(Camera* camera) const;
	//ジャンプ入力処理
	void input_jump();

	//オブジェクトをつかむ
protected:
	void on_landing()override;

	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;
	float move_speed = 15.0f;
	float turn_speed = DirectX::XMConvertToRadians(720);

	float jump_speed = 10.0f;
	int jump_count = 0;
	int jump_limit = 1;

	DirectX::XMFLOAT3 ending_target_pos{};

	//Effect* hitEffect = nullptr;
	std::unique_ptr<GPU_Particles> attack1;

	bool display_player_imgui = false;
/// </summary>
	//当たり判定用変数
	DirectX::XMFLOAT3 radius_aabb = { 5, 5, 5 };
};
