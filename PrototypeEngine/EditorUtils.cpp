#include "EditorUtils.h"


EditorUtils::EditorUtils()
{

}

bool EditorUtils::CreateScriptFile(const std::filesystem::path& folderPath, const std::string& scriptName)
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

bool EditorUtils::ReplaceInFile(const filesystem::path& filePath, const string& oldStr, const string& newStr)
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

bool EditorUtils::AddScriptFileToVcxProj(const filesystem::path& path, const string& scriptClassName)
{
    tinyxml2::XMLDocument doc;

    //1.XMLファイルをロード
    //stringのパスをC文字列(const char*)に変換して渡す
    if (doc.LoadFile(VCXPROJ_PATH.string().c_str()) != tinyxml2::XMLError::XML_SUCCESS)
    {
        Debug::Log("Error loading vcxproj file: %s\n", VCXPROJ_PATH.string().c_str());
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

    TIXMLASSERT(compileItemGroup != nullptr);

    tinyxml2::XMLElement* newClCompile = doc.NewElement("ClCompile");
    newClCompile->SetAttribute("Include", cppPath.c_str());

    compileItemGroup->InsertEndChild(newClCompile);

    // ----------------------------------------------------------------
    // 2. <ClInclude> に .h ファイルを追加 (IDE表示用)
    // ----------------------------------------------------------------
    // <ClInclude> タグを含む ItemGroup を検索/作成
    tinyxml2::XMLElement* headerItemGroup = FindOrCreateItemGroup("ClInclude");

    TIXMLASSERT(headerItemGroup != nullptr);

    tinyxml2::XMLElement* newClInclude = doc.NewElement("ClInclude");
    newClInclude->SetAttribute("Include", hPath.c_str());
    headerItemGroup->InsertEndChild(newClInclude);


    // 3. 変更をファイルに保存
    if (doc.SaveFile(VCXPROJ_PATH.string().c_str()) != tinyxml2::XML_SUCCESS)
    {
        return false;
    }

    return true;
}

bool EditorUtils::RemoveScriptFileToVcxProj(const filesystem::path& path, const string& scriptClassName)
{
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(VCXPROJ_PATH.string().c_str()) != tinyxml2::XMLError::XML_SUCCESS)return false;

    string rootPath = StringConverter::RemoveString(path.string(), path.filename().string());

    // 削除対象のパスを構築
    string cppPathToRemove = "..\\" + rootPath + scriptClassName + ".cpp";
    string hPathToRemove = "..\\" + rootPath + scriptClassName + ".h";
    std::replace(cppPathToRemove.begin(), cppPathToRemove.end(), '/', '\\');
    std::replace(hPathToRemove.begin(), hPathToRemove.end(), '/', '\\');

    tinyxml2::XMLElement* root = doc.RootElement();
    if (!root)return false;

    // フラグ
    bool changed = false;

    // 全ての <ItemGroup> をイテレート
    for (tinyxml2::XMLElement* itemGroup = root->FirstChildElement("ItemGroup");
        itemGroup != nullptr;
        itemGroup = itemGroup->NextSiblingElement("ItemGroup"))
    {
        // 1. <ClCompile(.cpp)> を削除
        tinyxml2::XMLElement* clCompile = itemGroup->FirstChildElement("ClCompile");
        while (clCompile) 
        {
            tinyxml2::XMLElement* next = clCompile->NextSiblingElement("ClCompile");
            const char* includeAttr = clCompile->Attribute("Include");
            if (includeAttr && (std::string(includeAttr)) == cppPathToRemove)
            {
                itemGroup->DeleteChild(clCompile);
                changed = true;
            }
            clCompile = next;
        }

        // 2. <ClInclude(.h)> を削除
        tinyxml2::XMLElement* clHeader = itemGroup->FirstChildElement("ClInclude");
        while (clHeader) {
            tinyxml2::XMLElement* next = clHeader->NextSiblingElement("ClInclude");
            const char* includeAttr = clHeader->Attribute("Include");
            if (includeAttr && (std::string(includeAttr)) == hPathToRemove) 
            {
                itemGroup->DeleteChild(clHeader);
                changed = true;
            }
            clHeader = next;
        }
    }

    if (changed) 
    {
        doc.SaveFile(VCXPROJ_PATH.string().c_str());
    }
    return true;
}
