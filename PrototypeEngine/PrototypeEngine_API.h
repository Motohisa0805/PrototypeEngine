#pragma once
#ifdef PROTOTYPEENGINE_EXPORTS // PrototypeEngineプロジェクトのビルド時
#define PROTOTYPEENGINE_API __declspec(dllexport)
#else // EngineCoreAppやGameScriptsから使用する時
#define PROTOTYPEENGINE_API __declspec(dllimport)
#endif