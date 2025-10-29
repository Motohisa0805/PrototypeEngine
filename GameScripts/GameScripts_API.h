#pragma once
#ifdef GAMESCRIPTS_EXPORTS
//DLLをビルドする時(GameScriptsプロジェクト内)：公開
#define GAMESCRIPTS_API __declspec(dllexport)
#else
//DLLを参照する時(Engineプロジェクト内)：取り込む
#define GAMESCRIPTS_API __declspec(dllimport)
#endif