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

			//各親無しアクターをループして表示
			for (auto& actor : actors)
			{
				//親がいないオブジェクトだけ描画
				if (actor->GetParentActor() == nullptr)
				{
					DrawActorNode(actor);
				}
			}

		}
		// ----------------------------------------------------------------
		// 2. パネルの空きスペースを右クリックしたときのメニュー
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
			}
			if (mSelectedActor)
			{
				if (ImGui::MenuItem("Release Parent Object"))
				{
					mSelectedActor->SetParent(nullptr);
				}

				if (ImGui::MenuItem("Delete Actor"))
				{
					SceneManager::GetNowScene()->DeleteActor(mSelectedActor);
					mSelectedActor = nullptr;
				}
			}
			if (ImGui::MenuItem("GUI Initialization of position"))
			{
				isResetLayout = true;
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void HierarchyPanel::DrawActorNode(ActorObject* actor)
{
	if (!actor || actor->GetState() == ActorObject::EDead)
	{
		return;
	}

	//ノードフラグの設定
	ImGuiBackendFlags node_flags = ImGuiTreeNodeFlags_AllowOverlap;
	bool isSelected = (mSelectedActor == actor);
	if (isSelected)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	//子オブジェクトを取得
	const vector<Transform*>& children = actor->GetChildActorList();

	//子オブジェクトがなければ末端ノードとして扱う
	if (children.empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	//PushIDでユニークIDを設定
	ImGui::PushID(actor);

	//ImGui::TreeNodeExを使用
	bool open = ImGui::TreeNodeEx(actor->GetName().c_str(), node_flags);

	//ノードがクリックされたら選択オブジェクトを更新
	if (ImGui::IsItemClicked())
	{
		mSelectedActor = actor;
	}

	//1.ドラッグ元(Drag Source)の設定
	if (ImGui::BeginDragDropSource())
	{
		//ペイロードとしてオブジェクトのポインターを格納
		ImGui::SetDragDropPayload("ACTOR_NODE_PTR", &actor, sizeof(ActorObject*));
		//ドラッグ中に表示されるテキスト
		ImGui::Text("%s", actor->GetName().c_str());
		ImGui::EndDragDropSource();
	}

	//2a.ドロップ先(Drop Target)の設定(子として追加)
	if (ImGui::BeginDragDropTarget())
	{
		//ドラッグペイロードを受け取る
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ACTOR_NODE_PTR"))
		{
			//ポインタのサイズが正しいことを確認
			if (payload->DataSize == sizeof(ActorObject*))
			{
				//ドラッグされてきたオブジェクトのポインターを取得
				ActorObject* draggedActor = *(ActorObject**)payload->Data;

				//ドロップ先オブジェクトが、ドラッグ元オブジェクト自身またはその子孫でないことを確認
				bool isCircular = false;
				Transform* parentCheck = actor;
				while (parentCheck != nullptr)
				{
					if (parentCheck == draggedActor)
					{
						isCircular = true;
						break;
					}
					parentCheck = parentCheck->GetParentActor();
				}

				if (!isCircular && draggedActor != actor)
				{
					// SetParentを呼び出すだけで、親子関係の付け替えが完結する
					draggedActor->SetParent(actor);

					mSelectedActor = draggedActor;
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	//ノードが開かれた場合、子オブジェクトを再帰的に描画
	if (open)
	{
		for (Transform* childTransform : children)
		{
			if (ActorObject* childActor = dynamic_cast<ActorObject*>(childTransform))
			{
				DrawActorNode(childActor);
			}
		}

		//子要素の描画が終了したら
		if (!(node_flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}

void HierarchyPanel::ClearPointer()
{
	mSelectedActor = nullptr;
}
