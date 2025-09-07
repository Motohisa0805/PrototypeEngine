#pragma once
#include "SDL3.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

//�O���錾
//�`��N���X
class Renderer;
//�c�[���o�[�̃p�l��
class ToolbarPanel;
//�V�[���r���[�̃p�l��
class SceneViewPanel;
//�Q�[���r���[�̃p�l��
class GameViewPanel;
//�q�G�����L�[�p�l��
class HierarchyPanel;
//�v���W�F�N�g�I��p�̃p�l��
class ProjectPanel;
//�A�C�e���I��p�̃p�l��
class SelectItemPanel;

class GUIWinMain
{
private:
	//�`��N���X
	static Renderer*				mRenderer;

	//***��ԊǗ�***
	//�Đ������ǂ���
	static bool						isPlaying;
	//�X�^�[�g���������u��
	static bool						isStarting;
	//�I����Ă邩�ǂ���
	static bool						isPaused;
	//�I������u�Ԃ��ǂ���
	static bool						isPushEnd;

	static bool						isFrameByFrame;

	//�Q�[���E�B���h�E�̃T�C�Y(��)
	static Vector2 					mGameWinPos;
	//�Q�[���E�B���h�E�̃T�C�Y(��)
	static Vector2 					mGameWinSize;

	static Vector2 					mSceneWinSize;

	//�c�[���o�[�̃p�l��
	static ToolbarPanel*			mToolbarPanel;
	//�V�[���r���[�̃p�l��
	static SceneViewPanel*			mSceneViewPanel;
	//�Q�[���r���[�̃p�l��
	static GameViewPanel*			mGameViewPanel;
	//�q�G�����L�[�p�l��
	static HierarchyPanel*			mHierarchyPanel;
	//�v���W�F�N�g�I��p�̃p�l��
	static ProjectPanel*			mProjectPanel;
	//�A�C�e���I��p�̃p�l��
	static SelectItemPanel*			mSelectItemPanel;

public:
	GUIWinMain() = default;
	~GUIWinMain() = default;
	// Initialize ImGui
	static bool InitializeImGui(SDL_Window* window, SDL_GLContext glContext);
	// Update ImGui state
	static void UpdateImGuiState();
	// Render ImGui
	static void RenderImGui();

	// Shutdown ImGui
	static void ShutdownImGui();

	static bool IsPlaying() { return isPlaying; }
	static void SetIsPlaying(bool playing) { isPlaying = playing; }
	static bool IsStarting() { return isStarting; }
	static void SetIsStarting(bool starting) { isStarting = starting; }
	static bool IsPaused() { return isPaused; }
	static void SetIsPaused(bool paused) { isPaused = paused; }
	static bool IsPushEnd() { return isPushEnd; }
	static void SetIsPushEnd(bool pushEnd) { isPushEnd = pushEnd; }
	static bool IsFrameByFrame() { return isFrameByFrame; }
	static void SetIsFrameByFrame(bool frameByFrame) { isFrameByFrame = frameByFrame; }

	static void SetRenderer(class Renderer* renderer) { mRenderer = renderer; }

	static Vector2 GetGameWinPos() { return mGameWinPos; }
	static void SetGameWinPos(const Vector2& pos) { mGameWinPos = pos; }
	static Vector2 GetGameWinSize() { return mGameWinSize; }
	static void SetGameWinSize(const Vector2& size) { mGameWinSize = size; }

	static Vector2 GetSceneWinSize() { return mSceneWinSize; }
	static void SetSceneWinSize(const Vector2& size) { mSceneWinSize = size; }

	static class ToolbarPanel* GetToolbarPanel() { return mToolbarPanel; }
	static class SceneViewPanel* GetSceneViewPanel() { return mSceneViewPanel; }
	static class GameViewPanel* GetGameViewPanel() { return mGameViewPanel; }
	static class HierarchyPanel* GetHierarchyPanel() { return mHierarchyPanel; }
	static class ProjectPanel* GetProjectPanel() { return mProjectPanel; }
	static class SelectItemPanel* GetSelectItemPanel() { return mSelectItemPanel; }
};

