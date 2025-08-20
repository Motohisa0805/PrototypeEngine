#pragma once
#include "Math.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍元のファイル
//ランダム出力を管理するクラス
class Random
{
private:
	static std::mt19937 sGenerator;
public:
	static void Init();

	// 指定された整数でジェネレーターをシードします
	// 注意：通常、手動で使用する必要はありません。
	static void			Seed(unsigned int seed);

	// 0.0fから1.0fの間の浮動小数点数を取得。
	static float		GetFloat();

	// 指定された範囲から浮動小数点数を取得
	static float		GetFloatRange(float min, float max);

	// 指定された範囲から整数を取得
	static int			GetIntRange(int min, int max);

	// 最小/最大の範囲を考慮してランダムなベクトルを取得
	static Vector2		GetVector(const Vector2& min, const Vector2& max);
	static Vector3		GetVector(const Vector3& min, const Vector3& max);

};