#pragma comment(lib, "Rpcrt4.lib")
#include <windows.h>
#include <filesystem>
#include "Typedefs.h"

namespace fs = std::filesystem;

class AssetImporter
{
private:

public:
    // GUIから呼ばれる用の関数。Assetsフォルダ内の全てのFBXファイルをチェックして、必要であればインポートする
	static void CheckAndImportAssets();
    // GUIから呼ばれる用の関数。1つのファイルをチェックして、必要であればインポートする
	static void OneFileCheckAndImportAssets(const filesystem::path& filePath);

	static string GenerateUUID();

	static fs::path GeneratedCustomPath(const fs::path& path);

	static void ConvertFBXToCustomFormat(const fs::path& fbxPath,
                                         const fs::path& customPath);
};
