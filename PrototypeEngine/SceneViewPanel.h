#pragma once
#include "EditorWindow.h"

//エディターシーンの描画処理クラス
//
class SceneViewPanel : public EditorWindow
{
private:
	class SceneEditorCamera*	mSceneEditorCamera;

	class SceneViewEditor*		mSceneViewEditor;
	class GBuffer*				mSceneBuffer;

	Vector2 					mSceneWinSize;

	bool						mIsShadowFrag;

	bool						mIsDebugGridFrag;
public:

								SceneViewPanel(Renderer* renderer);
								~SceneViewPanel();

	void						Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	bool						MouseHoveredDisble()override;

	void						Draw(float width, float height)override;

	void						InputUpdate()override;

	void						Update()override;

	class SceneEditorCamera*	GetSceneEditorCamera() { return mSceneEditorCamera; }

	class SceneViewEditor*		GetSceneViewEditor() { return mSceneViewEditor; }
	class GBuffer*				GetSceneBuffer() { return mSceneBuffer; }
	Vector2						GetSceneWinSize() { return mSceneWinSize; }
	void						SetSceneWinSize(const Vector2& size) { mSceneWinSize = size; }

	bool						IsShadowFrag() { return mIsShadowFrag; }

	bool						IsDebugGridFrag() { return mIsDebugGridFrag; }
};