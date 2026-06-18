#pragma once
#include "GUIEditorManager.h"
#include "EditorWindow.h"
#include "SceneEditorCamera.h"

//エディターシーンの描画処理クラス
//
class SceneViewPanel : public EditorWindow
{
private:
	SceneEditorCamera*			mSceneEditorCamera;

	SceneViewEditor*			mSceneViewEditor;
	GBuffer*					mSceneBuffer;

	Vector2 					mSceneWinSize;

	bool						mIsShadowFrag;

	bool						mIsDebugGridFrag;
public:

								SceneViewPanel(class Renderer* renderer);
								~SceneViewPanel();

	void						Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	bool						MouseHoveredDisble()override;

	void						Draw(float width, float height)override;

	void						InputUpdate()override;

	void						Update()override;

	SceneEditorCamera*			GetSceneEditorCamera() { return mSceneEditorCamera; }

	SceneViewEditor*			GetSceneViewEditor() { return mSceneViewEditor; }
	GBuffer*					GetSceneBuffer() { return mSceneBuffer; }
	Vector2						GetSceneWinSize() { return mSceneWinSize; }
	void						SetSceneWinSize(const Vector2& size) { mSceneWinSize = size; }

	bool						IsShadowFrag() { return mIsShadowFrag; }

	bool						IsDebugGridFrag() { return mIsDebugGridFrag; }
};