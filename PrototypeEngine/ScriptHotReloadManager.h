#pragma once
#include <Windows.h>//HMODULE用
#include "Entity.h"
#include "Typedefs.h"
#pragma comment(lib,"kernel32.lib")

#include <atomic>//ファイル監視スレッドの実行状態管理用
#include <thread>//ファイル監視スレッド用
#include <mutex>

enum class FileAction {
	Added,
	Removed,
	Rename,
	Modified
};
struct FileChangeEvent {
	string filePath;
	FileAction action;
};

//DLLから取得する関数ポインタの型定義
typedef Component* (*CreateScriptComponentFunc)(const char* ,Entity*);
typedef void (*UnregisterScriptComponentsFunc)();

struct DllHandle
{
	HMODULE hDll = nullptr;
	//DLLから取得した関数のポインタ
	CreateScriptComponentFunc CreateFunc = nullptr;
	UnregisterScriptComponentsFunc UnregisterFunc = nullptr;
};

struct SavedScriptsData
{
	string ClassName;	// 生成したスクリプト名
	json Data;			//Component::Serializeで得られたプロパティ値
};

struct PipeCloser
{
	void operator()(FILE* pipe)const
	{
		if (pipe)
		{
			_pclose(pipe);
		}
	}
};
using UniquePipe = std::unique_ptr<FILE, PipeCloser>;
// <要注意>ホットリロードはLauncherで起動したPrototypeEngine.exeでのみ正常に動作
//（Visual StudioのスタートアッププロジェクトをLauncherに設定して、そこからPrototypeEngine.exeを起動してください）
class ScriptHotReloadManager
{
private:

	float												mCheckFileTimer;

	std::map<std::string, FILETIME>						mKnownAssetTimestamps;
	bool												mFirstScanComplete = false;
	//ファイルロックされていない、ビルド出力先
	string												mSourceDllPath;
	// ロード中のDLL名
	string												mActiveDllPath;

	string												mSourcePDBPath;
	// ロード中のDLL名
	string												mActivePDBPath;
	// 次のロードでファイル名を変更するためのインデックス
	int													mDllIndex;
	DllHandle											mCurrentDll;
	// 現在ゲーム内に存在するすべてのエンティティとコンポーネントを保持するリスト（ホットリロード時に利用）
	std::map<Entity*, vector<SavedScriptsData>>			mSavedState;

	FILETIME											mLastLoadTime;

	FILETIME											GetDllLastWriteTime(const string& filePath);

	int													ExecuteAndWaitForProcess(const string& command);
	//.vcxproj更新フラグ
	std::atomic<bool>									mProjectUpdateNeeded;
	//スクリプトファイルが存在するフォルダーのパス
	const string										mScriptsDirectory;

	string												GetVsWherePath();

	string												FindMsBuildPath();
	//自動ビルドとリロード
	//DLLの変更がない場合でも、プロジェクトファイルが更新された場合にビルドを強制する関数
	bool												ExecuteMsbuildAndReload();
public:
	ScriptHotReloadManager();
	~ScriptHotReloadManager();

	bool												Initialize();
	//DLLをロードする関数
	bool												LoadScripts();
	//スクリプトコードを変更した際に呼び出す関数
	bool												ReloadInGameProject();
	//DLLをアンロードする関数
	void												UnloadScripts();

	bool												CheckForChanges(float deltaTime);
};

