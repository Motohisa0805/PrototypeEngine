@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

echo ===================================================
echo  PrototypeEngine - 起動スクリプト
echo ===================================================

:: 1. 実行中のカレントディレクトリをバッチファイルのある場所に固定
cd /d "%~dp0"

:: [オプション] 起動前に古いアクティブDLLが残っていれば削除（ホットリロードの衝突防止）
if exist "bin\InGameProject_Active.dll" (
    echo [INFO] 古いアクティブDLLをクリーンアップしています...
    del "bin\InGameProject_Active.dll"
    del "bin\InGameProject_Active.pdb" 2>nul
)

:: 2. エンジンの起動（binフォルダ内の本体を、ルート階層を作業ディレクトリとして実行）
echo [INFO] PrototypeEngine を起動しています...
start "" "bin\PrototypeEngine.exe"

endlocal