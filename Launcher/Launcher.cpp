// Launcher.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <windows.h>
#include <string>

int main()
{
    // 自分の実行パスを取得
    char buf[MAX_PATH];
    GetModuleFileNameA(NULL, buf, MAX_PATH);
    std::string exePath(buf);

    // 自分があるディレクトリを抽出
    size_t pos = exePath.find_last_of("\\/");
    std::string baseDir = exePath.substr(0, pos);

    // bin フォルダを組み立てる
    std::string binPath = baseDir + "\\bin";
    std::string targetExe = binPath + "\\PrototypeEngine.exe";

    // DLL 検索パスに bin を追加
    SetDllDirectoryA(binPath.c_str());

    // 本体 exe を起動
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    BOOL ok = CreateProcessA(
        targetExe.c_str(),   // 実行ファイル
        NULL,                // コマンドライン引数（必要ならここに書く）
        NULL, NULL, FALSE,
        0,
        NULL,
        binPath.c_str(),     // 作業ディレクトリを bin に設定
        &si,
        &pi
    );

    if (!ok) {
        MessageBoxA(NULL, "PrototypeEngine.exe を起動できませんでした", "Launcher Error", MB_OK);
        return 1;
    }

    // 親プロセスは終了してOK（待ちたければ WaitForSingleObject）
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}