#include "ProjectPanel.h"
#include "DebugManager.h"
#include "EditorSettingsManager.h"
#include "FileOperationManager.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
#include "ScriptEditManager.h"
#include "imgui_internal.h"
#include "AssetImporter.h"
#include "AssetDataBase.h"
#include "MaterialGenerater.h"

filesystem::path ProjectPanel::mPathToRename = "";

string ProjectPanel::mRenameInputBuffer = "";

bool ProjectPanel::mIsRenaming = false;

filesystem::path ProjectPanel::mSelectedFolderPath = "Assets";

char ProjectPanel::mScriptCreateBuffer[256] = "";

bool ProjectPanel::mIsShowScriptPopup = false;

ProjectPanel::ProjectPanel(Renderer* renderer) : EditorWindow(renderer)
{
    mID = "Project";
}

void ProjectPanel::Initialize(float width, float height, ImTextureRef ref)
{
    mWidthPos   = width * 0.65f;
    mHeightPos  = 55.0f;
    mWidthSize  = width * 0.15f;
    mHeightSize = height - 55.0f;
    EditorWindow::Initialize(width, height, ref);
    filesystem::path assetsPath = "Assets/";
    AssetDataBase::GetInstance().RefreshDataBase(assetsPath);
}

void ProjectPanel::Draw(float width, float height)
{
    EditorWindow::Draw(width, height);
    if (ImGui::Begin(GetImGuiWindowID().c_str(), &mIsShow,
                     ImGuiWindowFlags_NoCollapse))
    {

        if (mIsShowScriptPopup)
        {
            ImGui::OpenPopup("Create New Script");
            mIsShowScriptPopup = false;
        }

        DrawOverwritePopup();
        DrawScriptCreatePopup();

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            !ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
        {
            if (!ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
            {
                SelectionManager::GetSelectedFilePath().clear();
            }
        }

        // パネル全体の横幅を取得
        float totalWidth = ImGui::GetContentRegionAvail().x;

        float leftColumnWidth = totalWidth * 0.30f;
        float rightColumnWidth =
            totalWidth * 0.70f - ImGui::GetStyle().ItemSpacing.x;
        // フォルダツリー表示用のウィンドウ
        if (ImGui::BeginChild("FolderTree_Child", ImVec2(leftColumnWidth, 0.0f),
                              ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX))
        {
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "Right-click or [Assets] Menu click for options.");
            }
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            // 左カラム = フォルダツリー
            if (ImGui::TreeNode("Assets"))
            {
                // 左クリックで選択中フォルダを更新(Assetsフォルダ用)
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left) ||
                    ImGui::IsItemClicked(ImGuiMouseButton_Right))
                {
                    // 選択パスを更新
                    //SelectionManager::SetSelectedFilePath("Assets");
                    mSelectedFolderPath = "Assets";
                }

                // フォルダツリー表示
                DrawFolderTree("Assets");
                RightClickMenu();
                ImGui::TreePop();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if (ImGui::BeginChild("Assets_Child", ImVec2(0.0f, 0.0f),
                              ImGuiChildFlags_Border))
        {
            // 選択中フォルダの中身表示
            DrawPickUpFolderView();
            // 右クリック処理
            RightClickMenu();
        }
        ImGui::EndChild();

        //外部ファイルドロップのインポート処理
        //現在のProjectパネル状にマウスがあるか判定
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
        {
            if (!FileOperationManager::GetDroppedFiles().empty())
            {
                for (const auto& filePath :
                    FileOperationManager::GetDroppedFiles())
                {
                    // コピー元のパスを作成
                    filesystem::path srcPath = filePath;
                    // コピー先のパスを作成
                    filesystem::path dstPath =
                        mSelectedFolderPath / srcPath.filename();

                    try
                    {
                        // コピー先に同名のファイルがある場合は上書きする
                        filesystem::rename(srcPath, dstPath);
                        Debug::Log("Imported external file: %s\n",
                                   dstPath.string().c_str());
                        AssetImporter::OneFileCheckAndImportAssets(dstPath);
                    }
                    catch (const std::exception& e)
                    {
                        // Driveが異なる場合はコピー＆削除で対応する
                        try
                        {
                            // コピー先に同名のファイルがある場合は上書きする
                            filesystem::copy(srcPath, dstPath,filesystem::copy_options::overwrite_existing);
                            //コピーが成功したら元のファイルを削除する
                            filesystem::remove(srcPath);
                            Debug::Log("Imported (Copy & Delete) external file: %s\n",
                                dstPath.string().c_str());
                            AssetImporter::OneFileCheckAndImportAssets(dstPath);
                        }
                        catch (const std::exception& e)
                        {
                            Debug::Log("Import failed: %s\n", e.what());
                        }
                        Debug::Log("Import failed: %s\n", e.what());
                    }
                }
            }
        }
    }

    ImGui::Separator();
    if (!SelectionManager::GetSelectedFilePath().empty())
    {
        ImGui::Text("Selection: %s", SelectionManager::GetSelectedFilePath().string().c_str());
    }
    else
    {
        ImGui::Text("Selection: None");
    }

    ImGui::End();
    FileOperationManager::ClearDroppedFiles();
}

void ProjectPanel::DrawFolderTree(const filesystem::path& path)
{
    for (auto& entry : filesystem::directory_iterator(path))
    {
        if (!entry.is_directory())
            continue; // フォルダだけ表示

        const string folderName = entry.path().filename().string();

        // 名前変更処理
        if (mIsRenaming && entry.path() == mPathToRename)
        {
            RenameFunction(entry);
        }
        else
        {
            // ツリーノードの表示
            // ImGuiTreeNodeFlags_Selected:
            // mSelectedPathと一致する場合にハイライト表示させる
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanAvailWidth;
            if (entry.path() == SelectionManager::GetSelectedFilePath() ||
                entry.path() == mSelectedFolderPath)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }
            bool pushedColor = false;
            if (entry.path() == SelectionManager::GetSelectedFilePath() ||
                entry.path() == mSelectedFolderPath)
            {
                // 現在のテーマの「ホバー時の色」をベースとして取得する
                ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered];

                // アルファ値（透明度）を 1.0f
                color.w = 1.0f;

                ImGui::PushStyleColor(ImGuiCol_Header, color);
                pushedColor = true;
            }

            bool open = ImGui::TreeNodeEx(folderName.c_str(), flags);

            // 左クリックで選択中フォルダを更新
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) ||
                ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
               // SelectionManager::SetSelectedFilePath(entry.path()); // 選択パスを更新
                mSelectedFolderPath = entry.path();
            }

            if (pushedColor)
            {
                ImGui::PopStyleColor();
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
    if (!filesystem::exists(mSelectedFolderPath))
        return;

    // === パンくずリスト表示 ===
    filesystem::path root     = mSelectedFolderPath;
    filesystem::path relative = filesystem::relative(mSelectedFolderPath, root);
    // ルート ("Assets") を必ず表示
    if (ImGui::Button("Assets##2"))
    {
        mSelectedFolderPath = root;
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
            mSelectedFolderPath = temp;
        }
    }

    ImGui::Separator(); // パンくずとファイルリストを区切る

    float windowVisibleX2 =
        ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    float itemWidth =
        64.0f + ImGui::GetStyle().ItemSpacing.x; // アイコンの幅＋余白

    for (auto& entry : filesystem::directory_iterator(mSelectedFolderPath))
    {
        //ファイルを表示するかしないかチェックする
        if (entry.path().extension() == ".meta")
        {
            continue;
        }

        //ファイル表示処理
        DrawFileSystemEntry(entry);

        // 次のアイテムが現在のウィンドウ幅に収まる場合のみ、横に並べる（収まらないなら自動改行）
        float lastItemX2 = ImGui::GetItemRectMax().x;
        float nextItemX2 = lastItemX2 + itemWidth;
        if (nextItemX2 < windowVisibleX2)
        {
            ImGui::SameLine();
        }
    }
}

void ProjectPanel::DrawFileSystemEntry(const filesystem::directory_entry& entry)
{
    const string name       = entry.path().filename().string();
    bool         isSelected = (SelectionManager::GetSelectedFilePath() == entry.path());

    // 各アイテムをグループ化し、一意のIDで包む（これで競合とバグを完全に防ぐ）
    ImGui::PushID(entry.path().string().c_str());
    ImGui::BeginGroup();

    // 選択中のアセットはボタンの背景色を変える（Unityの青背景を選択風に再現）
    if (isSelected)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
    }

    //ファイルの拡張子などに応じて、表示するテクスチャ（ID）を切り替える
    ImTextureID iconTextureID =
        (ImTextureID)(uintptr_t)EditorTextureManager::GetInstance()
            .GetFileIconTexture(entry.path().string(),
                                entry.path().extension().string())
            ->GetTextureID();

    if (ImGui::ImageButton("##icon", iconTextureID, ImVec2(64, 64)))
    {
        SelectionManager::SetSelectedFilePath(entry.path());
    }
    if (isSelected)
    {
        ImGui::PopStyleColor();
    }
    // 4. ボタンの下にファイル名を表示（Unity風グリッド）
    ImGui::PushItemWidth(64); // アイコンと同じ幅にする
    // リネーム処理
    if (mIsRenaming && entry.path() == mPathToRename)
    {
        RenameFunction(entry);
        ImGui::PopItemWidth();
        ImGui::EndGroup(); // 一旦グループを閉じる
        ImGui::PopID();    // IDの破棄
    }
    else
    {
        ImGui::TextWrapped(name.c_str()); // 長い名前は折り返す
        ImGui::PopItemWidth();

        ImGui::EndGroup(); // 一旦グループを閉じる
        DragDropFunction(entry.path());
        if (ImGui::IsItemHovered())
        {
            // 左ダブルクリック処理
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (entry.is_directory())
                {
                    //SelectionManager::SetSelectedFilePath(entry.path());
                    mSelectedFolderPath = entry.path();
                }
                else
                {
                    if (entry.path().extension().string() == ".json")
                    {
                        SceneManager::LoadSceneGUI(entry.path().string());
                    }
                    else
                    {
                        FileOperationManager::OpenFile(entry.path());
                    }
                }
            }

            // 右クリックされた瞬間、そのアセットを選択状態にする（Unity互換）
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                SelectionManager::SetSelectedFilePath(entry.path());
            }
        }
        //FBXファイルをドラッグアンドドロップした時の処理
        // メッシュ用のドラッグ&ドロップ元
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            // パスとメッシュ名の両方をペイロードに含める
            string payloadData = entry.path().string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM",
                                      payloadData.c_str(),
                                      payloadData.size() + 1);
            ImGui::Text("%s", entry.path().string().c_str());
            ImGui::EndDragDropSource();
        }

        //FBXファイルの場合、中身を展開表示するドロップダウン
        if (entry.path().extension() == ".fbx")
        {
            //ツリーノードで展開できるようにする
            if (ImGui::TreeNodeEx("##fbx_contents", ImGuiTreeNodeFlags_SpanAvailWidth, "Contents"))
            {
                vector<SubMeshPayload> subMeshes;
                if (AssetDataBase::GetInstance().GetSubMeshs(entry.path(), subMeshes))
                {
                    for (size_t i = 0; i < subMeshes.size(); ++i)
                    {
                        ImGui::PushID(static_cast<int>(i));
                        ImGui::BeginGroup();
                        ImTextureID fbxItemIconID =
                            (ImTextureID)(uintptr_t)
                                EditorTextureManager::GetInstance()
                                    .GetFileIconTexture(entry.path().string(),".bank")->GetTextureID();

                        ImGui::ImageButton("##fbxItemIcon", fbxItemIconID,
                                           ImVec2(54, 54));
                        // サブアイテムの描画
                        ImGui::TextWrapped(subMeshes[i].sSubMeshName);
                        ImGui::EndGroup();

                        // サブメッシュ用のドラッグ&ドロップ元
                        if (ImGui::BeginDragDropSource(
                                ImGuiDragDropFlags_SourceAllowNullID))
                        {
                            SubMeshPayload payloadData = {};
                            // パスとサブメッシュ名を安全にコピー
                            strncpy_s(payloadData.sSubMeshName,
                                      sizeof(payloadData.sSubMeshName),
                                      entry.path().string().c_str(), _TRUNCATE);
                            strncpy_s(payloadData.sLocalID,
                                      sizeof(payloadData.sLocalID),
                                      subMeshes[i].sLocalID, _TRUNCATE);
                            ImGui::SetDragDropPayload("SUB_MESH_ITEM",
                                                      &payloadData,
                                                      sizeof(payloadData));
                            ImGui::Text("Mesh: %s", subMeshes[i].sSubMeshName);
                            ImGui::EndDragDropSource();
                        }
                        ImGui::PopID();
                    }
                }
                ImGui::TreePop();
            }
        }


        // コンテキストメニュー、ショートカット、ドラッグ＆ドロップ
        // DragDropFunction(entry.path());
        ShortcutKeyInputFunction(entry.path());

        ImGui::PopID(); // IDの破棄
    }
}

void ProjectPanel::SetDockWindow(ImGuiID id, ImGuiID& outID)
{

    ImGuiID dock_id_folderTree =
        ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.5f, NULL, NULL);
    ImGui::DockBuilderDockWindow("FolderTree", dock_id_folderTree);
    ImGuiID dock_id_assets =
        ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.5f, NULL, NULL);
    ImGui::DockBuilderDockWindow("Assets", dock_id_assets);
    outID = dock_id_assets;
}

bool ProjectPanel::RightClickMenu()
{
    if (SelectionManager::GetSelectedFilePath().empty())
        return false;
    SetPopupColorTheme();
    // コンテキストメニューは直前に描画したアイテム（TreeNode か
    // Selectable）に紐づく
    if (ImGui::BeginPopupContextWindow("ProjectContext",
                                       ImGuiMouseButton_Right))
    {
        if (ImGui::BeginMenu("Create"))
        {
            // フォルダ、シーン、スクリプトの作成
            CreateNewFolder();
            CreateNewMaterial();
            CreateNewScene("Scene");
            CreateNewScript();
            ImGui::EndMenu();
        }
        // Show in Explorer（フォルダ・ファイルどちらでも可）
        ShowInExplorer();
        // Open（ファイルのみ）
        if (!filesystem::is_directory(SelectionManager::GetSelectedFilePath()))
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
    // フォルダ
    if (ImGui::MenuItem("Folder"))
    {
        // 簡易的に NewFolder を作る (衝突は考慮していない)
        try
        {
            // 簡易的なユニーク名生成の例
            std::string uniqueName = "NewFolder";
            int         counter    = 1;
            while (filesystem::exists(mSelectedFolderPath / uniqueName))
            {
                uniqueName = "NewFolder (" + std::to_string(counter++) + ")";
            }
            filesystem::create_directory(mSelectedFolderPath / uniqueName);
        }
        catch (const exception& e)
        {
            Debug::Log("Create folder failed: %s\n", e.what());
        }
    }
}

void ProjectPanel::CreateNewMaterial()
{
    if (ImGui::MenuItem("Material"))
    {
        string uniqueName = "NewMaterial.mat";
        filesystem::path targetFolder = mSelectedFolderPath;
        if (mSelectedFolderPath.has_extension())
        {
            targetFolder = mSelectedFolderPath.parent_path();
        }

        // 既に存在するファイル名かチェックし、ユニークな名前に変更する
        int counter = 1;
        while (filesystem::exists(targetFolder / uniqueName))
        {
            // NewScene(1).json, NewScene(2).json のように生成
            uniqueName = "NewMaterial (" + std::to_string(counter++) + ").mat";
        }

        filesystem::path newMatPath = targetFolder / uniqueName;

        //マテリアルファイル作成処理
        if (MaterialGenerater::GeneratedBlankMaterial(newMatPath))
        {
            // 成功ログ
            Debug::Log("Created new mat file: %s\n", newMatPath.string().c_str());

            SelectionManager::SetSelectedFilePath(newMatPath);
            RenameStart();
        }
        else
        {
            // 失敗ログ
            Debug::Log("Failed to create mat file: %s\n",
                       newMatPath.string().c_str());
        }
    }
}

void ProjectPanel::CreateNewScene(const string& name, bool loadScene)
{
    // シーン作成
    if (ImGui::MenuItem(name.c_str()))
    {
        string           uniqueName = "NewScene.json"; // 拡張子付きで初期化
        filesystem::path targetFolder =
            mSelectedFolderPath; // 現在右クリックしているパス（フォルダ）
        if (mSelectedFolderPath.has_extension())
        {
            targetFolder = mSelectedFolderPath.parent_path();
        }

        // 既に存在するファイル名かチェックし、ユニークな名前に変更する
        int counter = 1;
        while (filesystem::exists(targetFolder / uniqueName))
        {
            // NewScene(1).json, NewScene(2).json のように生成
            uniqueName = "NewScene (" + std::to_string(counter++) + ").json";
        }

        filesystem::path newScenePath = targetFolder / uniqueName;

        // 3. SceneSerializerを使って空のシーンデータをファイルに書き出す
        // SceneSerializer::SaveEmptyScene()内でファイル書き込み処理を行う
        if (SceneSerializer::CreateEmptyScene(newScenePath))
        {
            // 成功ログ
            Debug::Log("Created new scene: %s\n",
                       newScenePath.string().c_str());
            if (loadScene)
            {
                SceneManager::LoadSceneGUI(newScenePath.string());
            }
        }
        else
        {
            // 失敗ログ
            Debug::Log("Failed to create scene file: %s\n",
                       newScenePath.string().c_str());
        }
    }
}

void ProjectPanel::CreateNewScript()
{
    // Script作成
    if (ImGui::MenuItem("Script (C++)"))
    {
        mScriptCreateBuffer[0] = '\0';

        mIsShowScriptPopup = true;
    }
}

void ProjectPanel::ShowInExplorer()
{
    if (ImGui::MenuItem("Show in Explorer"))
    {
        std::filesystem::path selectedPath = SelectionManager::GetSelectedFilePath();

        // もし何も選択されていなければ現在のフォルダを開く（フォールバック）
        if (selectedPath.empty())
        {
            selectedPath = ProjectPanel::GetSelectedFolderPath();
        }

        FileOperationManager::ShowInExplorer(selectedPath.wstring());
    }
}

void ProjectPanel::OpenFile()
{
    if (ImGui::MenuItem("Open"))
    {
        // ファイルの場合
        if (SelectionManager::GetSelectedFilePath().extension().string() == ".json")
        {
            // シーンファイルのロード処理を呼び出す
            // 実行中のシーンと切り替えるため、SceneManagerに処理を依頼します
            SceneManager::LoadSceneGUI(SelectionManager::GetSelectedFilePath().string());
            // EditorSettingsManager::GetInstance().SetLastOpenedScene(entry.path().string());
        }
        else if (filesystem::is_directory(SelectionManager::GetSelectedFilePath()))
        {
            mSelectedFolderPath = SelectionManager::GetSelectedFilePath();
        }
        else
        {
            FileOperationManager::OpenFile(
                SelectionManager::GetSelectedFilePath());
        }
    }
}

void ProjectPanel::DeleteFileOrFolder()
{
    // 削除メニューはAssetsフォルダ自体を削除できないようにする
    ImGui::BeginDisabled(SelectionManager::GetSelectedFilePath() == "Assets");
    // フォルダの削除
    if (ImGui::MenuItem("Delete"))
    {
        // 即削除はしない。遅延キューに追加する
        EditorSettingsManager::SetDeleteDirectoryQueue(SelectionManager::GetSelectedFilePath());
        EditorSettingsManager::ProcessScriptDelete(SelectionManager::GetSelectedFilePath());
    }
    ImGui::EndDisabled();
}

void ProjectPanel::RenameMenu()
{
    if (ImGui::MenuItem("Rename"))
    {
        RenameStart();
    }
}

void ProjectPanel::CopyPathMenu()
{
    if (ImGui::MenuItem("Copy Path"))
    {
        ImGui::SetClipboardText(SelectionManager::GetSelectedFilePath().string().c_str());
        Debug::Log("Copied path to clipboard: %s\n",
                   SelectionManager::GetSelectedFilePath().string().c_str());
    }
}

void ProjectPanel::ShortcutKeyInputFunction(const filesystem::path& path)
{
    if (!WindowHoveredConfirmation())
    {
        return;
    }
    // 削除キー
    if (!SelectionManager::GetSelectedFilePath().empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        EditorSettingsManager::SetDeleteDirectoryQueue(SelectionManager::GetSelectedFilePath());
        EditorSettingsManager::ProcessScriptDelete(SelectionManager::GetSelectedFilePath());
    }
    // 名前変更キー
    if (!SelectionManager::GetSelectedFilePath().empty() &&
        ImGui::IsKeyPressed(ImGuiKey_F2))
    {
        RenameStart();
    }
}

void ProjectPanel::DragDropFunction(const filesystem::path& path)
{
    const string& filePath = path.string();
    // ドラッグ開始処理（Selectable の近くに置く）
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", filePath.c_str(),
                                  filePath.size() + 1);
        // ドラッグ中の表示
        ImGui::Text("%s", path.filename().string().c_str());
        ImGui::EndDragDropSource();
    }
    // ドラッグ終了処理
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            if (payload->Data == nullptr || payload->DataSize <= 1)
                return;
            const char* srcPathC = (const char*)payload->Data;
            string      srcStr(srcPathC, payload->DataSize - 1);

            filesystem::path src(srcStr);
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
                    mPendingSrc         = src;
                    mPendingDst         = dst;
                    mShowOverwritePopup = true;
                    ImGui::OpenPopup("Overwrite?");
                }
                else
                {
                    try
                    {
                        filesystem::rename(src, dst);
                        //.metaファイルも移動
                        filesystem::path srcMeta = src.string() + ".meta";
                        filesystem::path dstMeta = dst.string() + ".meta";
                        if (filesystem::exists(srcMeta))
                        {
                            filesystem::rename(srcMeta, dstMeta);
                        }
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
    // ここで入力を行っている
    strncpy_s(buffer, mRenameInputBuffer.c_str(), sizeof(buffer));

    buffer[sizeof(buffer) - 1] = '\0';

    ImGuiID inputID = ImGui::GetID("##rename");

    // InputTextの設定。フォーカスを自動で当てる処理を入れておくと快適になります
    if (!ImGui::IsAnyItemActive())
    {
        ImGui::ActivateItemByID(inputID);
    }

    if (ImGui::InputText("##rename", buffer, sizeof(buffer),
                         ImGuiInputTextFlags_EnterReturnsTrue |
                             ImGuiInputTextFlags_AutoSelectAll))
    {
        std::string newName = buffer;

        FileOperationManager::ExecuteRename(entry.path(), newName);

        mIsRenaming = false;
    }

    // Esc キャンセル
    if (ImGui::IsItemDeactivated() && !ImGui::IsItemDeactivatedAfterEdit() ||
        ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        mIsRenaming = false;
    }

    ImGui::PopID();
}

void ProjectPanel::DrawOverwritePopup()
{
    if (ImGui::BeginPopupModal("Overwrite?", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("File already exists:\n%s\n\nOverwrite?",
                    mPendingDst.string().c_str());

        if (ImGui::Button("Yes", ImVec2(120, 0)))
        {
            try
            {
                filesystem::path srcMeta = mPendingSrc.string() + ".meta";
                filesystem::path dstMeta = mPendingDst.string() + ".meta";

                if (filesystem::exists(mPendingDst))filesystem::remove(mPendingDst);
                filesystem::rename(mPendingSrc, mPendingDst);

                //.metaファイルの上書き処理
                if (filesystem::exists(srcMeta))
                {
                    if (filesystem::exists(dstMeta))filesystem::remove(dstMeta);
                    filesystem::rename(srcMeta, dstMeta);
                }

                Debug::Log("Overwritten: %s -> %s\n",
                           mPendingSrc.string().c_str(),
                           mPendingDst.string().c_str());
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

void ProjectPanel::DrawScriptCreatePopup()
{
    if (ImGui::BeginPopupModal("Create New Script", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter Script Name (Do not include extension):");

        // ポップアップが開いた瞬間に、入力欄に自動でフォーカスを当てる
        if (ImGui::IsWindowAppearing() && !ImGui::IsAnyItemActive())
        {
            ImGui::SetKeyboardFocusHere();
        }

        bool isSubmitted = ImGui::InputText(
            "##scriptName", mScriptCreateBuffer, sizeof(mScriptCreateBuffer),
            ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Create", ImVec2(120, 0)) || isSubmitted)
        {
            string scriptName = mScriptCreateBuffer;
            if (!scriptName.empty())
            {
                bool success =
                    ScriptEditManager::GetInstance().CreateScriptFile(
                        SelectionManager::GetSelectedFilePath(), scriptName);

                if (success)
                {
                    Debug::Log("Successfully created script: %s\n",
                               scriptName.c_str());

                    string hPath = scriptName + "h";
                    ScriptEditManager::GetInstance().AddScriptFileToVcxProj(
                        SelectionManager::GetSelectedFilePath() / hPath, scriptName);
                }
                else
                {
                    Debug::Log("Failed to create script: %s\n",
                               scriptName.c_str());
                }
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ProjectPanel::RenameStart() 
{
    mPathToRename = SelectionManager::GetSelectedFilePath();
    // ファイルなら拡張子を除いた stem を編集バッファに、フォルダは full
    // name
    if (filesystem::is_directory(SelectionManager::GetSelectedFilePath()))
    {
        mRenameInputBuffer =
            SelectionManager::GetSelectedFilePath().filename().string();
    }
    else
    {
        mRenameInputBuffer =
            SelectionManager::GetSelectedFilePath().stem().string();
    }
    mIsRenaming = true;
}
