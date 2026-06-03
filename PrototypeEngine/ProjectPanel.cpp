#include "ProjectPanel.h"
#include "EditorSettingsManager.h"
#include "ScriptEditManager.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
#include "DebugManager.h"
#include "FileOperationManager.h"

filesystem::path ProjectPanel::mPathToRename = "";

string ProjectPanel::mRenameInputBuffer = "";

bool ProjectPanel::mRenaming = false;

filesystem::path ProjectPanel::mSelectedPath = "Assets";

ProjectPanel::ProjectPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void ProjectPanel::Initialize(float width, float height, ImTextureRef ref)
{
    mWidthPos = width * 0.65f;
    mHeightPos = 55.0f;
    mWidthSize = width * 0.15f;
    mHeightSize = height - 55.0f;
    GUIPanel::Initialize(width, height, ref);
}

void ProjectPanel::Draw(float width, float height, ImTextureRef ref)
{
    float panel1_SizeWidth = mWidthSize / 2.0f;
    // ウインドウ位置とサイズを固定
    if (isResetLayout)
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
        ImGui::SetNextWindowSize(ImVec2(panel1_SizeWidth, mHeightSize));
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(panel1_SizeWidth, mHeightSize), ImGuiCond_Once);
    }
	//フォルダツリー表示用のウィンドウ
    if (ImGui::Begin("FolderTree", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Right-click or [Assets] Menu click for options.");
        }
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        // 左カラム = フォルダツリー
        if (ImGui::TreeNode("Assets"))
        {
            // 左クリックで選択中フォルダを更新(Assetsフォルダ用)
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
				EditorSettingsManager::SetCurrentFolder("Assets");
				mSelectedPath = "Assets"; // 選択パスを更新
            }

            //フォルダツリー表示
            DrawFolderTree("Assets");
            RightClickMenu();
            ImGui::TreePop();
        }
    }
    ImGui::End();

    // ウインドウ位置とサイズを固定
    if (isResetLayout)
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos + panel1_SizeWidth, mHeightPos));
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
        isResetLayout = false;
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos + panel1_SizeWidth, mHeightPos), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(panel1_SizeWidth, mHeightSize), ImGuiCond_Once);
    }
	// 右カラム = 選択中フォルダの中身
    if (ImGui::Begin(EditorSettingsManager::GetCurrentFolder().string().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
    {
        //選択中フォルダの中身表示
        DrawPickUpFolderView();
        //右クリック処理
        RightClickMenu();
    }
    ImGui::End();

    DrawOverwritePopup();
}

void ProjectPanel::DrawFolderTree(const filesystem::path& path)
{
    for (auto& entry : filesystem::directory_iterator(path))
    {
        if (!entry.is_directory()) continue; // フォルダだけ表示

        const string name = entry.path().filename().string();
        
        //名前変更処理
        if (mRenaming && entry.path() == mPathToRename)
        {
            RenameFunction(entry);
        }
        else
        {
            // ツリーノードの表示
            // ImGuiTreeNodeFlags_Selected: mSelectedPathと一致する場合にハイライト表示させる
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (path == mSelectedPath)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            bool open = ImGui::TreeNodeEx(name.c_str(), flags); 

            // 左クリックで選択中フォルダを更新
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)|| ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
				EditorSettingsManager::SetCurrentFolder(entry.path());
                mSelectedPath = entry.path(); // 選択パスを更新
            }

            // 通常のフォルダ用メニュー（削除・リネーム可）
            // 右クリックメニュー（Rename 選択で mRenaming が true になる）
            ShortcutKeyInputFunction(entry.path());
            if (open)
            {
                DrawFolderTree(entry.path());
                ImGui::TreePop();
            }

            DragDropFunction(entry.path());
        }
    }
}
// 選択中フォルダの中身を表示(右カラム)
void ProjectPanel::DrawPickUpFolderView()
{
    if (!filesystem::exists(EditorSettingsManager::GetCurrentFolder())) return;

    // === パンくずリスト表示 ===
    filesystem::path root = EditorSettingsManager::GetCurrentFolder();
    filesystem::path relative = filesystem::relative(EditorSettingsManager::GetCurrentFolder(), root);
    // ルート ("Assets") を必ず表示
    if (ImGui::Button("Assets##2"))
    {
		EditorSettingsManager::SetCurrentFolder(root);
    }

    filesystem::path temp = root;
    for (auto& part : relative)
    {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();

        temp /= part;
        if (ImGui::Button(part.string().c_str()))
        {
			EditorSettingsManager::SetCurrentFolder(temp);
        }
    }

    ImGui::Separator(); // パンくずとファイルリストを区切る

    for (auto& entry : filesystem::directory_iterator(EditorSettingsManager::GetCurrentFolder()))
    {
        DrawFileSystemEntry(entry);
    }
}

void ProjectPanel::DrawFileSystemEntry(const filesystem::directory_entry& entry)
{
    const string name = entry.path().filename().string();

    // リネーム処理
    if (mRenaming && entry.path() == mPathToRename)
    {
        RenameFunction(entry);
        return; // リネーム中は以降の処理を行わない
    }

    // -- - 通常の表示-- -
    ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_None;
    if (entry.path() == mSelectedPath)
    {
        selectableFlags |= ImGuiSelectableFlags_Highlight; // ハイライト表示
    }

    // フォルダ・ファイル選択 (単一クリックでの選択と移動)
    // この処理で、フォルダでもファイルでも mSelectedPath は更新される
    if (ImGui::Selectable(name.c_str(), false, selectableFlags))
    {
        mSelectedPath = entry.path();

        // シングルクリックでフォルダ移動させたくない場合はこのブロックを削除
        if (!entry.is_directory())
        {
            // ファイル選択
            mCurrentFile = entry.path().string();
        }
    }

    // ダブルクリック処理
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (entry.is_directory())
        {
            // フォルダの場合のみ移動を実行
            EditorSettingsManager::SetCurrentFolder(entry.path());

            // ダブルクリックで移動した場合、選択状態も更新する
            mSelectedPath = entry.path();
        }
        else
        {
            // ファイルの場合
            if (entry.path().extension().string() == ".json")
            {
                // シーンファイルのロード処理を呼び出す
                // 実行中のシーンと切り替えるため、SceneManagerに処理を依頼します
                SceneManager::LoadSceneGUI(entry.path().string());
                //EditorSettingsManager::GetInstance().SetLastOpenedScene(entry.path().string());
            }
            else
            {
                // その他のファイルの場合（外部エディタで開くなど）
				FileOperationManager::OpenFile(entry.path());
            }
        }
    }
    // コンテキストメニュー、ショートカット、ドラッグ＆ドロップ
    ShortcutKeyInputFunction(entry.path()); 
    DragDropFunction(entry.path());
}

bool ProjectPanel::RightClickMenu()
{
	if (mSelectedPath.empty()) return false;
    SetPopupColorTheme();
    // コンテキストメニューは直前に描画したアイテム（TreeNode か Selectable）に紐づく
    if (ImGui::BeginPopupContextWindow("ProjectContext", ImGuiMouseButton_Right))
    {
        if (ImGui::BeginMenu("Create")) {
			//フォルダ、シーン、スクリプトの作成
            CreateNewFolder();
			CreateNewScene();
			CreateNewScript();
            ImGui::EndMenu();
        }
		// Show in Explorer（フォルダ・ファイルどちらでも可）
		ShowInExplorer();
		// Open（ファイルのみ）
        if (!filesystem::is_directory(mSelectedPath))
        {
            OpenFile();
		}
		// Delete（フォルダ・ファイルどちらでも可。ただしAssetsフォルダ自体は削除できない）
		DeleteFileOrFolder();
        // Rename（フォルダ・ファイルどちらでも可）
        RenameMenu();
		// Copy Path（フォルダ・ファイルどちらでも可）
        CopyPathMenu();
        ImGui::Separator();

        ImGui::EndPopup();
    }
	ResetPopupColorTheme();
    return true;
}

void ProjectPanel::CreateNewFolder()
{
    //フォルダ
    if (ImGui::MenuItem("Folder"))
    {
        // 簡易的に NewFolder を作る (衝突は考慮していない)
        try
        {
            // 簡易的なユニーク名生成の例
            std::string uniqueName = "NewFolder";
            int counter = 1;
            while (filesystem::exists(mSelectedPath / uniqueName)) {
                uniqueName = "NewFolder (" + std::to_string(counter++) + ")";
            }
            filesystem::create_directory(mSelectedPath / uniqueName);

        }
        catch (const exception& e)
        {
            Debug::Log("Create folder failed: %s\n", e.what());
        }
    }
}

void ProjectPanel::CreateNewScene()
{
    //シーン作成
    if (ImGui::MenuItem("Scene"))
    {
        string uniqueName = "NewScene.json"; // 拡張子付きで初期化
        filesystem::path targetFolder = mSelectedPath; // 現在右クリックしているパス（フォルダ）

        // 既に存在するファイル名かチェックし、ユニークな名前に変更する
        int counter = 1;
        while (filesystem::exists(targetFolder / uniqueName)) {
            // NewScene(1).json, NewScene(2).json のように生成
            uniqueName = "NewScene (" + std::to_string(counter++) + ").json";
        }

        filesystem::path newScenePath = targetFolder / uniqueName;

        // 3. SceneSerializerを使って空のシーンデータをファイルに書き出す
        // SceneSerializer::SaveEmptyScene()内でファイル書き込み処理を行う
        if (SceneSerializer::SaveEmptyScene(newScenePath))
        {
            // 成功ログ
            Debug::Log("Created new scene: %s\n", newScenePath.string().c_str());
        }
        else
        {
            // 失敗ログ
            Debug::Log("Failed to create scene file: %s\n", newScenePath.string().c_str());
        }
    }
}

void ProjectPanel::CreateNewScript()
{
    //Script作成
    if (ImGui::MenuItem("Script (C++)"))
    {
        // 1. 一時的な名前でリネームモードを開始する
        // （この"PendingNewScript"はファイル名ではなく、UI上の状態を示すID）
        filesystem::path tempPath = mSelectedPath / "PendingNewScript.h";

        mSelectedPath = tempPath;
        mPathToRename = tempPath;
        mRenameInputBuffer = "New Script"; // デフォルトの入力文字列
        mRenaming = true;
    }
}

void ProjectPanel::ShowInExplorer()
{
    if (ImGui::MenuItem("Show in Explorer"))
    {
        std::filesystem::path selectedPath = mSelectedPath;

        // もし何も選択されていなければ現在のフォルダを開く（フォールバック）
        if (selectedPath.empty()) {
            selectedPath = EditorSettingsManager::GetCurrentFolder();
        }

        FileOperationManager::ShowInExplorer(selectedPath.wstring());
    }
}

void ProjectPanel::OpenFile()
{
    if (ImGui::MenuItem("Open"))
    {
        if (filesystem::is_directory(mSelectedPath)) {
            EditorSettingsManager::SetCurrentFolder(mSelectedPath);
        }
        else {
            FileOperationManager::OpenFile(mSelectedPath);
        }
    }
}

void ProjectPanel::DeleteFileOrFolder()
{
    //削除メニューはAssetsフォルダ自体を削除できないようにする
    ImGui::BeginDisabled(mSelectedPath == "Assets");
    //フォルダの削除
    if (ImGui::MenuItem("Delete Folder"))
    {
        // 即削除はしない。遅延キューに追加する
        EditorSettingsManager::SetDeleteQueue(mSelectedPath);
        EditorSettingsManager::ProcessScriptDelete(mSelectedPath);
    }
    ImGui::EndDisabled();
}

void ProjectPanel::RenameMenu()
{
    if (ImGui::MenuItem("Rename"))
    {
        mPathToRename = mSelectedPath;
        // ファイルなら拡張子を除いた stem を編集バッファに、フォルダは full name
        if (filesystem::is_directory(mSelectedPath))
        {
            mRenameInputBuffer = mSelectedPath.filename().string();
        }
        else
        {
            mRenameInputBuffer = mSelectedPath.stem().string();
        }
        mRenaming = true;
    }
}

void ProjectPanel::CopyPathMenu()
{
    if (ImGui::MenuItem("Copy Path"))
    {
        ImGui::SetClipboardText(mSelectedPath.string().c_str());
        Debug::Log("Copied path to clipboard: %s\n", mSelectedPath.string().c_str());
	}
}

void ProjectPanel::ShortcutKeyInputFunction(const filesystem::path& path)
{
    if (!WindowHoveredConfirmation()) { return; }
    //削除キー
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
		EditorSettingsManager::SetDeleteQueue(mSelectedPath);
        EditorSettingsManager::ProcessScriptDelete(mSelectedPath);
    }
    //名前変更キー
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_F2))
    {
        mPathToRename = mSelectedPath; //mSelectedPath をターゲットに

        // ファイルなら拡張子を除いた stem を編集バッファに、フォルダは full name
        if (filesystem::is_directory(mSelectedPath))
        {
            mRenameInputBuffer = mSelectedPath.filename().string();
        }
        else
        {
            mRenameInputBuffer = mSelectedPath.stem().string();
        }
        mRenaming = true;
    }
}

void ProjectPanel::DragDropFunction(const filesystem::path& path)
{
    const string& filePath = path.string();
    // ドラッグ開始処理（Selectable の近くに置く）
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", filePath.c_str(), filePath.size() + 1);
        // ドラッグ中の表示
        ImGui::Text("%s", path.filename().string().c_str());
        ImGui::EndDragDropSource();
    }
    //ドラッグ終了処理
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char* srcPathC = (const char*)payload->Data;
            filesystem::path src(srcPathC);
            filesystem::path dst;

            if (filesystem::is_directory(path))
            {
                // フォルダにドロップ → 中に入れる
                dst = path / src.filename();
            }
            else
            {
                // ファイルにドロップ → 同じ階層
                dst = path.parent_path() / src.filename();
            }

            if (src != dst && !src.string().starts_with(dst.string()))
            {
                if (filesystem::exists(dst))
                {
                    // すでに存在 → 上書き確認ダイアログへ
                    mPendingSrc = src;
                    mPendingDst = dst;
                    mShowOverwritePopup = true;
                    ImGui::OpenPopup("Overwrite?");
                }
                else
                {
                    try
                    {
                        filesystem::rename(src, dst);
                    }
                    catch (const exception& e)
                    {
                        Debug::Log("Move failed: %s\n", e.what());
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void ProjectPanel::RenameFunction(const filesystem::directory_entry entry)
{
    ImGui::PushID(entry.path().string().c_str());

    char buffer[256];
    //ここで入力を行っている
    strncpy_s(buffer, mRenameInputBuffer.c_str(), sizeof(buffer));

    buffer[sizeof(buffer) - 1] = '\0';

    // InputTextの設定。フォーカスを自動で当てる処理を入れておくと快適になります
    if (ImGui::IsWindowAppearing() && !ImGui::IsAnyItemActive()) {
        ImGui::SetKeyboardFocusHere();
    }

    if (ImGui::InputText("##rename", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::string newName = buffer;

		FileOperationManager::ExecuteRename(entry.path(), newName);

        mRenaming = false;
    }

    // Esc キャンセル
    if (ImGui::IsItemDeactivated() && !ImGui::IsItemDeactivatedAfterEdit() || ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        mRenaming = false;
    }

    ImGui::PopID();
}

void ProjectPanel::DrawOverwritePopup()
{
    if (ImGui::BeginPopupModal("Overwrite?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("File already exists:\n%s\n\nOverwrite?", mPendingDst.string().c_str());

        if (ImGui::Button("Yes", ImVec2(120, 0)))
        {
            try
            {
                filesystem::remove(mPendingDst);            // 既存を消す
                filesystem::rename(mPendingSrc, mPendingDst); // 移動
                Debug::Log("Overwritten: %s -> %s\n", mPendingSrc.string().c_str(), mPendingDst.string().c_str());
            }
            catch (const exception& e)
            {
                Debug::Log("Overwrite failed: %s\n", e.what());
            }
            mShowOverwritePopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(120, 0)))
        {
            mShowOverwritePopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}