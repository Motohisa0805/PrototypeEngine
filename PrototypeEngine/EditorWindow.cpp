#include "EditorWindow.h"
#include "DebugManager.h"
#include "WindowRenderProperty.h"

EditorWindow::EditorWindow(Renderer* renderer)
    : mRenderer(renderer)
    , isMouseHovered(false)
    , isResetLayout(false)
    , mID("EditorWindow")
    , mInstanceID(0)
{
}

EditorWindow::~EditorWindow()
{
    for (auto window : mChildren)
    {
        delete window;
        window = nullptr;
    }
    mChildren.clear();
}

string EditorWindow::GetImGuiWindowID() const
{
    if (mInstanceID == 0)
    {
        return mID;
    }
    // "WindowView###WindowView_1" のように ### を使って見た目と内部IDを分離する
    return mID + "###" + mID + "_" + std::to_string(mInstanceID);
}

void EditorWindow::Initialize(float width, float height, ImTextureRef ref)
{
    SetPanelColorTheme();
}

void EditorWindow::ResetLayoutFunction()
{
    // ウインドウ位置とサイズを固定
    if (isResetLayout)
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
        isResetLayout = false;
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize),
                                 ImGuiCond_Once);
    }
}

void EditorWindow::SetPanelColorTheme()
{
    ImGuiStyle& style  = ImGui::GetStyle();
    ImVec4*     colors = style.Colors;

    // 全体の背景と基本要素
    colors[ImGuiCol_WindowBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f); // パネル背景
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // パネル背景
    colors[ImGuiCol_Border] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // パネル背景
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // パネル背景
    colors[ImGuiCol_FrameBgHovered] =
        ImVec4(0.29f, 0.29f, 0.29f, 1.00f); // パネル背景
    colors[ImGuiCol_FrameBgActive] =
        ImVec4(0.35f, 0.35f, 0.35f, 1.00f); // パネル背景

    // タイトルバー
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // 通常
    colors[ImGuiCol_TitleBgActive] =
        ImVec4(0.13f, 0.13f, 0.13f, 1.00f); // アクティブ
    colors[ImGuiCol_TitleBgCollapsed] =
        ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // 通常

    // ボタン
    colors[ImGuiCol_Button]        = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonActive]  = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

    // ハイライト
    colors[ImGuiCol_Header] =
        ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // 非選択時のヘッダー/ノード
    colors[ImGuiCol_HeaderHovered] =
        ImVec4(0.26f, 0.59f, 0.98f, 0.65f); // ホバー時の青
    colors[ImGuiCol_HeaderActive] =
        ImVec4(0.26f, 0.59f, 0.98f, 1.00f); // アクティブ時の青
    colors[ImGuiCol_CheckMark] =
        ImVec4(0.26f, 0.59f, 0.98f, 1.00f); // チェックマーク

    // その他の設定
    style.WindowRounding    = 0.0f; // ウィンドウの角の丸さ
    style.FrameRounding     = 0.0f; // フレームの角を丸くしない
    style.GrabRounding      = 5.0f; // スライダーのグラブを丸くしない
    style.ScrollbarRounding = 0.0f; // スクロールバーを丸くしない
    style.WindowTitleAlign =
        ImVec2(0.5f, 0.5f); // タイトルを中央寄せ（Unity風）
}

bool EditorWindow::MouseHoveredDisble()
{
    isMouseHovered = false;
    return true;
}

void EditorWindow::ResetWindowPos(float width, float height) {}

bool EditorWindow::WindowHoveredConfirmation()
{
    // マウスがこのウィンドウにあるかどうか判定
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        // SceneViewパネルにマウスが乗っているときの処理
        isMouseHovered = true;
        return true;
    }
    return false;
}

void EditorWindow::Draw(float width, float height) {}

void EditorWindow::AddEditorWindow(EditorWindow* window)
{
    mChildren.push_back(window);
    window->Initialize(WindowRenderProperty::GetWidth(),
                       WindowRenderProperty::GetHeight());
    window->Enabled();
}

void EditorWindow::RemoveEditorWindow(EditorWindow* window)
{
    auto it = std::find(mChildren.begin(), mChildren.end(), window);
    if (it != mChildren.end())
    {
        mChildren.erase(it);
    }
    else
    {
        Debug::Log("Not founded this window");
    }
}

EditorWindow* EditorWindow::GetEditorWindowChild(int index)
{
    if (index < 0 || index >= mChildren.size())
    {
        return nullptr;
    }
    return mChildren[index];
}

void EditorWindow::BaseGUIPanelPopupMenu()
{
    // 2. 直前のアイテムに対するコンテキストメニューの定義
    if (ImGui::BeginPopupContextWindow())
    {
        // ここがポップアップメニューの中身
        // ポップアップが開いている間だけ描画されます

        if (ImGui::MenuItem("GUI Initialization of position"))
        {
            isResetLayout = true;
        }

        // 3. ポップアップの終了
        ImGui::EndPopup();
    }
    if (isMouseHovered)
    {
    }
}

void EditorWindow::ClearPointer() {}

float EditorWindow::GetWindowSizeWidth()
{
    float width = WindowRenderProperty::GetWidth();

    return width;
}

float EditorWindow::GetWindowSizeHeight()
{
    float height = WindowRenderProperty::GetHeight();
    return height;
}

ImVec2 EditorWindow::GetAspectRatio()
{
    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float  W_a            = available_size.x;
    float  H_a            = available_size.y;

    float W_d, H_d; // 描画サイズ

    // 幅を基準にした場合の高さ
    float H_test = W_a * (9.0f / 16.0f);

    if (H_test <= H_a)
    {
        // 縦方向に余裕があるため、幅を最大限に利用
        W_d = W_a;
        H_d = H_test;
    }
    else
    {
        // 横方向に余裕があるため、高さを最大限に利用
        H_d = H_a;
        W_d = H_a * (16.0f / 9.0f);
    }

    // 描画サイズが確定したので、中央寄せのためのオフセットを計算

    // オフセット量を計算
    float offset_x = (W_a - W_d) * 0.5f;
    float offset_y = (H_a - H_d) * 0.5f;

    // カーソル位置をオフセット分だけ移動させる
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset_y);

    return ImVec2(W_d, H_d);
}

void EditorWindow::SetPopupColorTheme()
{
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void EditorWindow::ResetPopupColorTheme() { ImGui::PopStyleColor(2); }
