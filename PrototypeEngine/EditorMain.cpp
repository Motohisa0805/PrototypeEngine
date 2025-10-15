#include "EngineWindow.h"
#include "SDL3.h"
#include <windows.h>
#define DISABLE_DEBUG_NEW // 一時的に無効にして
#include "Debug_memory.h"
#undef DISABLE_DEBUG_NEW   // すぐに解除

void CurrentDirectorySetting()
{
	// 実行ファイルのフルパスを取得（例: C:\Users\User\Project\bin\app.exe）
	char buf[MAX_PATH];
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	std::string exePath(buf);

	// 実行ファイルがあるディレクトリを抽出（例: C:\Users\User\Project\bin）
	size_t lastSlashPos = exePath.find_last_of("\\/");
	std::string parentDir = exePath.substr(0, lastSlashPos);

	// 削除したい文字列（\bin）を検索
	size_t binPos = parentDir.rfind("\\bin");

	// 文字列が見つかった場合
	if (binPos != std::string::npos) {
		// \binの部分をパスから削除（例: C:\Users\User\Project）
		std::string projectRoot = parentDir.substr(0, binPos);
		SetCurrentDirectory(projectRoot.c_str()); // カレントディレクトリをプロジェクトのルートに変更
		std::cout << "Project Root: " << projectRoot << std::endl;
	}
	else {
		std::cout << "Could not find '\\bin' in the path." << std::endl;
	}
}


//FOCUS : プロジェクトを実行している場所
//構成マネージャーがDebugなら
#ifdef _DEBUG
int main(int argc, char* argv[])
{
	CurrentDirectorySetting();


	// メモリリーク検出を有効にする
	EnableMemoryLeakCheck();
	// エンジンの初期化
	EngineWindow engine;
	bool success = engine.EngineInitialize();
	if (success)
	{
		engine.EngineRunLoop();
	}
	engine.EngineShutdown();
	return 0;
}
// プロジェクトの実行場所がReleaseなら
#elif defined(_RELEASE)
#define SDL_MAIN_HANDLED
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CurrentDirectorySetting();


	// メモリリーク検出を有効にする
	//EnableMemoryLeakCheck();
	// エンジンの初期化
	EngineWindow engine;
	bool success = engine.EngineInitialize();
	if (success)
	{
		engine.EngineRunLoop();
	}
	engine.EngineShutdown();
	return 0;
}
#endif