#pragma once
#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <filesystem>

class FileOperationManager
{
private:
	static void RenameScriptPair(const std::filesystem::path& oldPath, const std::string& newName);
	static void RenameNormalFileOrFolder(const std::filesystem::path& oldPath, const std::string& newName);
public:
	static void Initialize();

	static void ShowInExplorer(const std::wstring& filePath);

	static void OpenFile(const std::filesystem::path& path);

	static void Release();

	static void ExecuteRename(const std::filesystem::path& oldPath, const std::string& newName);

	static void OpenSceneDialog();
};

