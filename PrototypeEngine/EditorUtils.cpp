#include "EditorUtils.h"


EditorUtils::EditorUtils()
{

}

bool EditorUtils::CreateScriptFile(const std::filesystem::path& folderPath, const std::string& scriptName)
{
    //テンプレートの内容
    //TODO : まだテンプレート内容は出力してない
    //現在は仮でコード内で記述
    //1.ヘッダーファイル(.h)テンプレート
    string headerTemplate = R"(#pragma once
#include "PrototypeEngine/ScriptComponent.h"
// #include "Actor.h" など、必要に応じてインクルード

class [CLASS_NAME] : public ScriptComponent
{
public:
    // コンストラクタ
    [CLASS_NAME](class ActorObject* owner);

    // ライフサイクル関数のオーバーライド
    void Start() override;
    void Update(float deltaTime) override;
};
)";


    //2.ソースファイル(.cpp)テンプレート
    string cppTemplate = R"(#include "[CLASS_NAME].h"
#include <iostream>

[CLASS_NAME]::[CLASS_NAME](ActorObject* owner)
    : ScriptComponent(owner) // 基底クラスのコンストラクタを呼び出す
{
    mName = "[CLASS_NAME]"; // Inspectorでの表示名を設定
}

void [CLASS_NAME]::Start()
{
    // 初期化ロジックをここに記述
    std::cout << mName << "::Start() called!" << std::endl;
}

void [CLASS_NAME]::Update(float deltaTime)
{
    // 毎フレームの更新ロジックをここに記述
}


// ----------------------------------------------------------------
// 自動登録の実行
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT([CLASS_NAME]); // クラス名を渡すだけで自動登録される
)";


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

bool EditorUtils::AddScriptFileToVcxProj(const string& scriptClassName)
{
    tinyxml2::XMLDocument doc;

    //1.XMLファイルをロード
    //stringのパスをC文字列(const char*)に変換して渡す
    if (doc.LoadFile(VCXPROJ_PATH.string().c_str()) != tinyxml2::XMLError::XML_SUCCESS)
    {
        Debug::Log("Error loading vcxproj file: %s\n", VCXPROJ_PATH.string().c_str());
        return false;
    }

    // .cpp と .h のパスを構築
    string cppPath = "..\\" + SCRIPTS_ROOT_PATH + scriptClassName + ".cpp";
    string hPath = "..\\" + SCRIPTS_ROOT_PATH + scriptClassName + ".h";
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

bool EditorUtils::RemoveScriptFileToVcxProj(const string& scriptClassName)
{
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(VCXPROJ_PATH.string().c_str()) != tinyxml2::XMLError::XML_SUCCESS)return false;

    // 削除対象のパスを構築
    string cppPathToRemove = "..\\" + SCRIPTS_ROOT_PATH + scriptClassName + ".cpp";
    string hPathToRemove = "..\\" + SCRIPTS_ROOT_PATH + scriptClassName + ".h";
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
