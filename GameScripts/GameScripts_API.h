#pragma once
#ifdef GAMESCRIPTS_EXPORTS
//DLLをビルドする時(GameScriptsプロジェクト内)：公開
#define GAMESCRIPTS_API __declspec(dllexport)
#else
//DLLを参照する時(Engineプロジェクト内)：取り込む
#define GAMESCRIPTS_API __declspec(dllimport)
#endif

extern "C" GAMESCRIPTS_API void fibonacci_init(
    const unsigned long long a, const unsigned long long b);

// Produce the next value in the sequence.
// Returns true on success and updates current value and index;
// false on overflow, leaves current value and index unchanged.
extern "C" GAMESCRIPTS_API bool fibonacci_next();

// Get the current value in the sequence.
extern "C" GAMESCRIPTS_API unsigned long long fibonacci_current();

// Get the position of the current value in the sequence.
extern "C" GAMESCRIPTS_API unsigned fibonacci_index();