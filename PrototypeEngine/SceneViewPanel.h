#pragma once
#include "GUIEditorManager.h"
#include "EditorWindow.h"
#include "SceneEditorCamera.h"

//エディターシーンの描画処理クラス
//
class SceneViewPanel : public EditorWindow
{
private:
	static SceneEditorCamera*	mSceneEditorCamera;
public:

	SceneViewPanel(class Renderer* renderer);
	~SceneViewPanel();

	void						Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	bool						MouseHoveredDisble()override;

	void						Draw(float width, float height)override;

	static void					InputCameraUpdate();

	static void					CameraUpdate();

	static SceneEditorCamera*	GetSceneEditorCamera() { return mSceneEditorCamera; }
};

