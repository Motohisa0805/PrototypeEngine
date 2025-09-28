#include "GUIPanel.h"

GUIPanel::GUIPanel(Renderer* renderer)
	: mRenderer(renderer)
	, isMouseHovered(false)
	, isResetLayout(false)
{
}

void GUIPanel::Initialize(float width, float height, ImTextureRef ref)
{
}

bool GUIPanel::MouseHoveredDisble()
{
	isMouseHovered = false;
	return true;
}

void GUIPanel::ResetWindowPos(float width, float height)
{
}

bool GUIPanel::WindowHoveredConfirmation()
{
	//マウスがこのウィンドウにあるかどうか判定
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		// SceneViewパネルにマウスが乗っているときの処理
		isMouseHovered = true;
		return true;
	}
	return false;
}

void GUIPanel::Draw(float width, float height, ImTextureRef ref)
{

}

void GUIPanel::GUIPanelMenu()
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

float GUIPanel::GetWindowSizeWidth()
{
	float width = WindowRenderProperty::GetWidth();

	return width;
}

float GUIPanel::GetWindowSizeHeight()
{
	float height = WindowRenderProperty::GetHeight();
	return height;
}

ImVec2 GUIPanel::GetAspectRatio()
{
	ImVec2 available_size = ImGui::GetContentRegionAvail();
	float W_a = available_size.x;
	float H_a = available_size.y;

	float W_d, H_d; // 描画サイズ

	// 幅を基準にした場合の高さ
	float H_test = W_a * (9.0f / 16.0f);

	if (H_test <= H_a) {
		// 縦方向に余裕があるため、幅を最大限に利用
		W_d = W_a;
		H_d = H_test;
	}
	else {
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

	return ImVec2(W_d,H_d);
}
