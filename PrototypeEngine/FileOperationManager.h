#pragma once
#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <filesystem>

class FileOperationManager
{
public:
	static void Initialize();

	static void ShowInExplorer(const std::wstring& filePath);

	static void OpenFile(const std::filesystem::path& path);

	static void Release();
};

