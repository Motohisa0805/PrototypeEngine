#pragma once

namespace Physics
{
	//重力加速度
	constexpr float GRAVITY_ACCELERATION = 9.81f;

	constexpr int VELOCITY_ITERATIONS = 8; // 速度（跳ね返り・摩擦）の反復回数

	constexpr int POSITION_ITERATIONS = 3; // 位置（めり込み押し出し）の反復回数
}