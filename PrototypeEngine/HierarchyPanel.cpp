#include "HierarchyPanel.h"
#include "SceneManager.h" // SceneManager::GetNowScene() を使うために必要
#include "Actor.h"        // new ActorObject() を使うために必要

HierarchyPanel::HierarchyPanel(Renderer* renderer)
	:GUIPanel(renderer)
	, mSelectedActor(nullptr)
{
}

HierarchyPanel::~HierarchyPanel()
{
	if (mSelectedActor)
	{
		mSelectedActor = nullptr;
	}
}

void HierarchyPanel::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = width * 0.5f;
	mHeightPos = 55.0f;
	mWidthSize = width * 0.15f;
	mHeightSize = height - 55.0f;
}

void HierarchyPanel::Draw(float width, float height, ImTextureRef ref)
{
    if (isResetLayout)
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
		isResetLayout = false;
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
    }
	//  新しいウィンドウの作成
	if(ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse))
	{
		GUIPanelMenu();

		// ----------------------------------------------------------------
		// 1. 現在のシーンのアクター一覧を表示する
		// ----------------------------------------------------------------
		BaseScene* currentScene = SceneManager::GetNowScene();
		if (currentScene)
		{
			string name = currentScene->GetName();
			//仮シーン名を表示
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "Scene:%s", currentScene->GetName().c_str());
			ImGui::Separator();
			// Sceneからアクターリストを取得する関数を呼び出す
			// BaseScene::GetActors() が必要
			const vector<ActorObject*>& actors = currentScene->GetActors();

			//各アクターをループして表示
			for (auto& actor : actors)
			{
				//ImGui::Selectable()を使ってアクター名を表示
				//選択状態はmSelectedActorと比較して設定
				bool isSelected = (mSelectedActor == actor);
				ImGui::PushID(actor);//同じ名前のアクターがいてもユニークIDになるようにする
				if (ImGui::Selectable(actor->GetName().c_str(), isSelected))
				{
					//2.クリックされたら選択中のアクターを更新
					mSelectedActor = actor;
				}
				ImGui::PopID();
			}

		}
		// ----------------------------------------------------------------
		// 3. パネルの空きスペースを右クリックしたときのメニュー
		// ----------------------------------------------------------------
		//ImGui::BeginPopupContextWindow() は、現在のウィンドウがフォーカスされている状態で
        // 右クリックされた場合にポップアップメニューを開始します。
		if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Create Empty Actor"))
			{
				//3.ActorObjectの生成とシーンへの追加

				// ActorObject::ActorObject() コンストラクタ内で以下の処理が行われている前提です。
				// 1. SceneManager::GetNowScene() を取得し mGame に設定
				// 2. mGame->AddActor(this); を呼び出し、現在のシーンの Actor リストに追加
				ActorObject* newActor = new ActorObject();
				mSelectedActor = newActor; // 新しく作ったアクターを自動で選択

				// (ログ出力)
				// Debug::Log("Created new Actor: %s\n", newActor->GetName().c_str());
			}

			//(オプション)
			if (ImGui::MenuItem("Create Mesh Actor"))
			{
				//TODO: メッシュアクター作成処理
			}

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void HierarchyPanel::ClearPointer()
{
	mSelectedActor = nullptr;
}
