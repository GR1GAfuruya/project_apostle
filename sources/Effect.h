#pragma once
#include "camera.h"
#include "game_object.h"
class Effect
{
public:
	Effect(const char* file_name);
	~Effect();

	//==============================================================
	// 
	// 構造体
	// 
	//==============================================================


	//==============================================================
	// 
	// public関数
	// 
	//==============================================================

	void play(DirectX::XMFLOAT3 pos);

	void stop();

	void set_position(DirectX::XMFLOAT3 pos);
	void set_scale(DirectX::XMFLOAT3 scale);

	void debug_gui(string id);
private:
	shared_ptr<GameObject> particle_system;
};