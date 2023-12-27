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
	// ç\ë¢ëÃ
	// 
	//==============================================================


	//==============================================================
	// 
	// publicä÷êî
	// 
	//==============================================================

	void play(DirectX::XMFLOAT3 pos);

	void stop();

	void set_position(DirectX::XMFLOAT3 pos);

	void debug_gui(string id);
private:
	shared_ptr<GameObject> psrticle_system;
};