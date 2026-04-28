#include "ScriptEditManager.h"
#include "DebugManager.h"
#include "ConvertNumToString.h"

ScriptEditManager::ScriptEditManager()
{
    mVcxppoj_Path = "InGameProject/InGameProject.vcxproj";
}

bool ScriptEditManager::CreateScriptFile(const std::filesystem::path& folderPath, const std::string& scriptName)
{
    //テンプレートの内容
    //現在は仮でコード内で記述
    //1.ヘッダーファイル(.h)テンプレート
    filesystem::path hPath = "Library/HeaderTemplate.txt";
    string headerTemplate = StringConverter::Read_entire_file_binary(hPath.string());
    StringConverter::Convert_crlf_to_lf(headerTemplate);

    filesystem::path cppPath = "Library/CppTemplate.txt";
    string cppTemplate = StringConverter::Read_entire_file_binary(cppPath.string());
    StringConverter::Convert_crlf_to_lf(cppTemplate);

    //クラス名置換処理
    auto replaceClassName = [](string& content, const string& name) {
        string target = "[CLASS_NAME]";
        size_t pos = content.find(target);
        while (pos != string::npos)
        {
            content.replace(pos, target.length(), name);
            pos = content.find(target, pos + name.length());
        }
    };

    //テンプレート内の[CLASS_NAME]を置き換え
    replaceClassName(headerTemplate, scriptName);
    replaceClassName(cppTemplate, scriptName);

    //3.ファイル書き出し
    try
    {
        //.hファイル書き出し
        ofstream headerFile(folderPath / (scriptName + ".h"));
        if (!headerFile.is_open())return false;
        headerFile << headerTemplate;
        headerFile.close();

        //.cppファイルの書き出し
        ofstream cppFile(folderPath / (scriptName + ".cpp"));
        if (!cppFile.is_open())return false;
        cppFile << cppTemplate;
        cppFile.close();
        return true;
    }
    catch (const exception& e)
    {
        Debug::Log("Script file creation failed: %s\n", e.what());
        return false;

    }
}

bool ScriptEditManager::ReplaceInFile(const filesystem::path& filePath, const string& oldStr, const string& newStr)
{
    std::ifstream fileIn(filePath);
    if (!fileIn.is_open())
    {
        return false;
    }

    std::stringstream buffer;
    buffer << fileIn.rdbuf();//ファイル全体をバッファに書き込む
    fileIn.close();

    string content = buffer.str();

    //文字列の置き換え処理
    size_t pos = content.find(oldStr);
    bool replaced = false;

    while (pos != string::npos)
    {
        content.replace(pos, oldStr.length(), newStr);
        pos = content.find(oldStr, pos + newStr.length());
        replaced = true;
    }

    if (!replaced)
    {
        //置き換え対象が見つからなかった場合は書き込み不要
        return true;
    }

    //ファイルに書き戻し
    ofstream fileOut(filePath, std::ios::trunc);//ios::truncで既存の内容を削除
    if (!fileOut.is_open())
    {
        return false;
    }

    fileOut << content;
    fileOut.close();


    return true;
}

bool ScriptEditManager::DoesEntryExist(tinyxml2::XMLElement* itemGroup, const char* tagName, const char* includePath)
{
    if (!itemGroup)return false;

    for (tinyxml2::XMLElement* element = itemGroup->FirstChildElement(tagName);
        element != nullptr;
        element = element->NextSiblingElement(tagName))
    {
        const char* attr = element->Attribute("Include");
        if (attr && (string(attr) == includePath))
        {
            return true;//存在した
        }
    }
    return false;//存在した
}

bool ScriptEditManager::AddScriptFileToVcxProj(const filesystem::path& path, const string& scriptClassName)
{
    tinyxml2::XMLDocument doc;

    //1.XMLファイルをロード
    //stringのパスをC文字列(const char*)に変換して渡す
    if (doc.LoadFile(mVcxppoj_Path.string().c_str()) != tinyxml2::XMLError::XML_SUCCESS)
    {
        Debug::Log("Error loading vcxproj file: %s\n", mVcxppoj_Path.string().c_str());
        return false;
    }

    string rootPath = StringConverter::RemoveString(path.string(), path.filename().string());

    // .cpp と .h のパスを構築
    string cppPath = "..\\" + rootPath + scriptClassName + ".cpp";
    string hPath = "..\\" + rootPath + scriptClassName + ".h";
    std::replace(cppPath.begin(), cppPath.end(), '/', '\\');
    std::replace(hPath.begin(), hPath.end(), '/', '\\');

    tinyxml2::XMLElement* root = doc.RootElement();
    if (!root) return false;

    // ----------------------------------------------------------------
    // ユーティリティ関数: <ItemGroup> を検索し、見つからなければ新しく作成する
    // ----------------------------------------------------------------
    auto FindOrCreateItemGroup = [&](const char* childTagName) -> tinyxml2::XMLElement*
    {
            //既存のItemGroupの中から、指定された子タグを持つものを探す
            for (tinyxml2::XMLElement* itemGroup = root->FirstChildElement("ItemGroup");
                itemGroup != nullptr;
                itemGroup = itemGroup->NextSiblingElement("ItemGroup"))
            {
                if (itemGroup->FirstChildElement(childTagName))
                {
                    return itemGroup;//発見
                }
            }
            //見つからなかった場合、新しいItemGroupを作成し、ルートに挿入
            tinyxml2::XMLElement* newGroup = doc.NewElement("ItemGroup");
            root->InsertEndChild(newGroup);
    };

    // ----------------------------------------------------------------
    // 1. <ClCompile> に .cpp ファイルを追加
    // ----------------------------------------------------------------
    // <ClCompile> タグを含む ItemGroup を検索/作成
    tinyxml2::XMLElement* compileItemGroup = FindOrCreateItemGroup("ClCompile");

    if (!compileItemGroup) return false; // FindOrCreateItemGroupがnullptrを返した場合の安全策

    // 修正：既存チェックを追加
    if (!DoesEntryExist(compileItemGroup, "ClCompile", cppPath.c_str()))
    {
        tinyxml2::XMLElement* newClCompile = doc.NewElement("ClCompile");
        newClCompile->SetAttribute("Include", cppPath.c_str());
        compileItemGroup->InsertEndChild(newClCompile);
    }

    // ----------------------------------------------------------------
    // 2. <ClInclude> に .h ファイルを追加 (IDE表示用)
    // ----------------------------------------------------------------
    // <ClInclude> タグを含む ItemGroup を検索/作成
    tinyxml2::XMLElement* headerItemGroup = FindOrCreateItemGroup("ClInclude");
    if (!headerItemGroup) return false;

    //  修正：既存チェックを追加
    if (!DoesEntryExist(headerItemGroup, "ClInclude", hPath.c_str()))
    {
        tinyxml2::XMLElement* newClInclude = doc.NewElement("ClInclude");
        newClInclude->SetAttribute("Include", hPath.c_str());
        headerItemGroup->InsertEndChild(newClInclude);
    }


    // 3. 変更をファイルに保存
    if (doc.SaveFile(mVcxppoj_Path.string().c_str()) != tinyxml2::XML_SUCCESS)
    {
        return false;
    }

    return true;
}

bool ScriptEditManager::RemoveScriptFileToVcxProj(const filesystem::path& path, const string& scriptClassName)
{
    tinyxml2::XMLDocument doc;

    // 1. XMLファイルをロード
    if (doc.LoadFile(mVcxppoj_Path.string().c_str()) != tinyxml2::XML_SUCCESS) return false;

    // 2. 削除対象の拡張子を確認し、タグを決定 (.cpp なら ClCompile, .h なら ClHeader)
    string ext = path.extension().string();
    const char* targetTag = nullptr;
    if (ext == ".cpp") targetTag = "ClCompile";
    else if (ext == ".h") targetTag = "ClInclude";

    if (!targetTag) return false; // 対象外の拡張子なら終了

    // 3. プロジェクトファイルからの相対パスを構築 (例: Assets/Test.cpp -> ..\Assets\Test.cpp)
    string targetPath = "..\\" + path.string();
    std::replace(targetPath.begin(), targetPath.end(), '/', '\\');

    tinyxml2::XMLElement* root = doc.RootElement();
    if (!root) return false;

    bool changed = false;

    // 4. 全ての <ItemGroup> を検索
    for (tinyxml2::XMLElement* itemGroup = root->FirstChildElement("ItemGroup");
        itemGroup != nullptr;
        itemGroup = itemGroup->NextSiblingElement("ItemGroup"))
    {
        // ターゲットとなるタグ (ClCompile か ClHeader) を検索して削除
        tinyxml2::XMLElement* element = itemGroup->FirstChildElement(targetTag);
        while (element)
        {
            tinyxml2::XMLElement* next = element->NextSiblingElement(targetTag);
            const char* includeAttr = element->Attribute("Include");

            // パスが一致したら削除
            if (includeAttr && (string(includeAttr)) == targetPath)
            {
                itemGroup->DeleteChild(element);
                changed = true;
            }
            element = next;
        }
    }

    // 5. 変更があれば保存
    if (changed)
    {
        doc.SaveFile(mVcxppoj_Path.string().c_str());
    }
    return true;
}
