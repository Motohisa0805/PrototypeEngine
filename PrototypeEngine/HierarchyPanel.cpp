#include "HierarchyPanel.h"
#include "SceneManager.h" // SceneManager::GetNowScene() を使うために必要
#include "Actor.h"        // new ActorObject() を使うために必要
#include "UIActor.h"

HierarchyPanel::HierarchyPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

HierarchyPanel::~HierarchyPanel()
{
}

void HierarchyPanel::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = width * 0.5f;
	mHeightPos = 55.0f;
	mWidthSize = width * 0.15f;
	mHeightSize = height - 55.0f;
	GUIPanel::Initialize(width, height, ref);
}

void HierarchyPanel::Draw(float width, float height, ImTextureRef ref)
{
	ResetLayoutFunction();
	//  新しいウィンドウの作成
	if(ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse))
	{

		// ----------------------------------------------------------------
		// 1. 現在のシーンのアクター一覧を表示する
		// ----------------------------------------------------------------
		BaseScene* currentScene = SceneManager::GetNowScene();
		if (currentScene)
		{
			//シーン名を表示
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "Scene:%s", currentScene->GetName().c_str());
			ImGui::Separator();
			
			if (ImGui::CollapsingHeader("GameWorld", ImGuiTreeNodeFlags_DefaultOpen)) {
				// Sceneからアクターリストを取得する関数を呼び出す
				// BaseScene::GetActors() が必要
				const vector<ActorObject*>& actors = currentScene->GetActorManager()->GetActors();

				//各親無しアクターをループして表示
				for (auto& actor : actors)
				{
					//親がいないオブジェクトだけ描画
					if (actor->GetTransform()->GetParentActor() == nullptr)
					{
						DrawActorNode(actor);
					}
				}
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			
			//UI階層
			if (ImGui::CollapsingHeader("UI Canvas", ImGuiTreeNodeFlags_DefaultOpen)) {
				// Sceneからアクターリストを取得する関数を呼び出す
				// BaseScene::GetActors() が必要
				const vector<UIActorObject*>& actors = currentScene->GetUIActorManager()->GetActors();

				//各親無しアクターをループして表示
				for (auto& actor : actors)
				{
					//親がいないオブジェクトだけ描画
					if (actor->GetRectTransform()->GetParentActor() == nullptr)
					{
						//UIアクターのノード描画関数を呼び出す
						DrawUIActorNode(actor);
					}
				}
			}

		}
		// ----------------------------------------------------------------
		// 2. パネルを右クリックしたときのメニュー
		// ----------------------------------------------------------------
		RightClickMenu();

		// --- コピー (Ctrl + C) ---
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))
		{
			if (SelectionManager::GetSelectedActor())
			{
				EditorClipboard::Copy(SelectionManager::GetSelectedActor());
			}
		}

		// --- ペースト (Ctrl + V) ---
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
		{
			if (EditorClipboard::HasCopiedActor())
			{
				// ペーストはシーンが変わるのでコマンドを介して実行
				auto cmd = std::make_unique<PasteActorCommand>();
				CommandManager::Execute(std::move(cmd));
			}
		}
		//名前変更のショートカットキー
		if (ImGui::IsKeyDown(ImGuiKey_F2)) {
			if (!EditorSettingsManager::IsRenaming() && SelectionManager::GetSelectedActor()) {
				EditorSettingsManager::SetRenameInputBuffer(SelectionManager::GetSelectedActor()->GetName());
				EditorSettingsManager::SetRenaming(true);
			}
		}
		//削除ショートカットキー
		if (ImGui::IsKeyDown(ImGuiKey_Delete)) {
			if (SelectionManager::GetSelectedActor()) {
				auto cmd = std::make_unique<DeleteCommand>(SelectionManager::GetSelectedActor());
				CommandManager::Execute(std::move(cmd));
			}
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
	//ImGuiTreeNodeFlags_SpanAvailWidth : 選択の幅をGUIパネルの幅と同じにする
	ImGuiBackendFlags node_flags = ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool isSelected = (SelectionManager::GetSelectedActor() == actor);
	if (isSelected)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	//子オブジェクトを取得
	const vector<ActorObject*>& children = actor->GetTransform()->GetChildActorList();
	//子オブジェクトがなければ末端ノードとして扱う
	if (children.empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	//PushIDでユニークIDを設定
	ImGui::PushID(actor);

	// ----------------------------------------------------------------
	// 選択中なら背景色を「不透明な全塗り」に上書きする
	// ----------------------------------------------------------------
	bool pushedColor = false;
	if (isSelected)
	{
		// 現在のテーマの「ホバー時の色」をベースとして取得する
		ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered];

		// アルファ値（透明度）を 1.0f 
		color.w = 1.0f;

		ImGui::PushStyleColor(ImGuiCol_Header, color);
		pushedColor = true;
	}

	//リネーム中の場合、InputTextを表示
	if (SelectionManager::GetSelectedActor() == actor && EditorSettingsManager::IsRenaming())
	{
		char buffer[256];
		//ここで入力を行っている
#if defined(_MSC_VER)
		strncpy_s(buffer, EditorSettingsManager::GetRenameInputBuffer().c_str(), sizeof(buffer));
#else
		std::strncpy(buffer, EditorSettingsManager::GetRenameInputBuffer().c_str(), sizeof(buffer));
#endif
		buffer[sizeof(buffer) - 1] = '\0';

		if (ImGui::InputText("##rename", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			auto cmd = std::make_unique<RenameCommand>(SelectionManager::GetSelectedActor(), string(buffer));
			CommandManager::Execute(std::move(cmd));
			EditorSettingsManager::SetRenaming(false);
		}

		// Esc キャンセル
		if (ImGui::IsItemDeactivated() && !ImGui::IsItemDeactivatedAfterEdit())
		{
			EditorSettingsManager::SetRenaming(false);
		}
	}
	//リネーム中でなければ通常のノード表示
	else
	{
		//ImGui::TreeNodeExを使用
		bool open = ImGui::TreeNodeEx(actor->GetName().c_str(), node_flags);

		//ノードがクリックされたら選択オブジェクトを更新
		if (ImGui::IsItemClicked(0)|| ImGui::IsItemClicked(1))
		{
			SelectionManager::SetSelectedActor(actor);
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
				ActorObject* draggedActor = *(ActorObject**)payload->Data;

				// ガード：自分自身、または自分の子孫への移動は無視する
				if (!ImGuiHelper::IsAncestorOf(draggedActor, actor))
				{
					float mouseClickY = ImGui::GetMousePos().y;
					float nodeRectMinY = ImGui::GetItemRectMin().y;
					float nodeRectMaxY = ImGui::GetItemRectMax().y;
					float nodeHeight = nodeRectMaxY - nodeRectMinY;

					if (mouseClickY < nodeRectMinY + nodeHeight * 0.25f || mouseClickY > nodeRectMinY + nodeHeight * 0.75f)
					{
						// ----------------------------------------------------------------
						// 【ケースA】隙間にドロップ（＝ターゲットと「同じ親」の階層に滑り込ませる）
						// ----------------------------------------------------------------
						ActorObject* desiredParent = actor->GetTransform()->GetParentActor();

						// ターゲットがその親のリストの何番目にいるかを取得
						auto& siblingList = desiredParent ? desiredParent->GetTransform()->GetChildActorListMutable()
							: SceneManager::GetNowScene()->GetActorManager()->GetActorsMutable();

						auto it = std::find(siblingList.begin(), siblingList.end(), actor);
						size_t targetIndex = std::distance(siblingList.begin(), it);

						// 上部ならその位置、下部なら次の位置
						size_t toIndex = (mouseClickY < nodeRectMinY + nodeHeight * 0.25f) ? targetIndex : targetIndex + 1;

						// 同じ親の中で後ろに動かす場合は、挿入位置モデルの仕様に合わせてインデックスを調整するロジックが必要あり、
						// 別階層からの移動であれば、現在は問題はなし。
						auto cmd = std::make_unique<ReparentAndReorderCommand>(draggedActor, desiredParent, toIndex);
						CommandManager::Execute(std::move(cmd));
					}
					else
					{
						// ----------------------------------------------------------------
						// 【ケースB】中央にドロップ（＝ターゲットの「直下（子供）」にする）
						// ----------------------------------------------------------------
						size_t toIndex = actor->GetTransform()->GetChildActorList().size(); // 子供リストの末尾

						auto cmd = std::make_unique<ReparentAndReorderCommand>(draggedActor, actor, toIndex);
						CommandManager::Execute(std::move(cmd));
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		//ノードが開かれた場合、子オブジェクトを再帰的に描画
		if (open)
		{
			for (ActorObject* childTransform : children)
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
	}

	if (pushedColor)
	{
		ImGui::PopStyleColor();
	}

	ImGui::PopID();
}

void HierarchyPanel::DrawUIActorNode(UIActorObject* actor)
{
	if (!actor || actor->GetState() == ActorObject::EDead)
	{
		return;
	}

	//ノードフラグの設定
	//ImGuiTreeNodeFlags_SpanAvailWidth : 選択の幅をGUIパネルの幅と同じにする
	ImGuiBackendFlags node_flags = ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool isSelected = (SelectionManager::GetSelectedActor() == actor);
	if (isSelected)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	//子オブジェクトを取得
	const vector<UIActorObject*>& children = actor->GetRectTransform()->GetChildActorList();
	//子オブジェクトがなければ末端ノードとして扱う
	if (children.empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	//PushIDでユニークIDを設定
	ImGui::PushID(actor);

	// ----------------------------------------------------------------
	// 選択中なら背景色を「不透明な全塗り」に上書きする
	// ----------------------------------------------------------------
	bool pushedColor = false;
	if (isSelected)
	{
		// 現在のテーマの「ホバー時の色」をベースとして取得する
		ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered];

		// アルファ値（透明度）を 1.0f 
		color.w = 1.0f;

		ImGui::PushStyleColor(ImGuiCol_Header, color);
		pushedColor = true;
	}

	//リネーム中の場合、InputTextを表示
	if (SelectionManager::GetSelectedActor() == actor && EditorSettingsManager::IsRenaming())
	{
		char buffer[256];
		//ここで入力を行っている
#if defined(_MSC_VER)
		strncpy_s(buffer, EditorSettingsManager::GetRenameInputBuffer().c_str(), sizeof(buffer));
#else
		std::strncpy(buffer, EditorSettingsManager::GetRenameInputBuffer().c_str(), sizeof(buffer));
#endif
		buffer[sizeof(buffer) - 1] = '\0';

		if (ImGui::InputText("##rename", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			auto cmd = std::make_unique<RenameCommand>(SelectionManager::GetSelectedActor(), string(buffer));
			CommandManager::Execute(std::move(cmd));
			EditorSettingsManager::SetRenaming(false);
		}

		// Esc キャンセル
		if (ImGui::IsItemDeactivated() && !ImGui::IsItemDeactivatedAfterEdit())
		{
			EditorSettingsManager::SetRenaming(false);
		}
	}
	//リネーム中でなければ通常のノード表示
	else
	{
		//ImGui::TreeNodeExを使用
		bool open = ImGui::TreeNodeEx(actor->GetName().c_str(), node_flags);

		//ノードがクリックされたら選択オブジェクトを更新
		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
		{
			SelectionManager::SetSelectedActor(actor);
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
				UIActorObject* draggedActor = *(UIActorObject**)payload->Data;

				// ガード：自分自身、または自分の子孫への移動は無視する
				if (!ImGuiHelper::IsAncestorOf_UIActor(draggedActor, actor))
				{
					float mouseClickY = ImGui::GetMousePos().y;
					float nodeRectMinY = ImGui::GetItemRectMin().y;
					float nodeRectMaxY = ImGui::GetItemRectMax().y;
					float nodeHeight = nodeRectMaxY - nodeRectMinY;

					if (mouseClickY < nodeRectMinY + nodeHeight * 0.25f || mouseClickY > nodeRectMinY + nodeHeight * 0.75f)
					{
						// ----------------------------------------------------------------
						// 【ケースA】隙間にドロップ（＝ターゲットと「同じ親」の階層に滑り込ませる）
						// ----------------------------------------------------------------
						UIActorObject* desiredParent = actor->GetRectTransform()->GetParentActor();

						// ターゲットがその親のリストの何番目にいるかを取得
						auto& siblingList = desiredParent ? desiredParent->GetRectTransform()->GetChildActorListMutable()
							: SceneManager::GetNowScene()->GetUIActorManager()->GetActorsMutable();

						auto it = std::find(siblingList.begin(), siblingList.end(), actor);
						size_t targetIndex = std::distance(siblingList.begin(), it);

						// 上部ならその位置、下部なら次の位置
						size_t toIndex = (mouseClickY < nodeRectMinY + nodeHeight * 0.25f) ? targetIndex : targetIndex + 1;

						// 同じ親の中で後ろに動かす場合は、挿入位置モデルの仕様に合わせてインデックスを調整するロジックが必要あり、
						// 別階層からの移動であれば、現在は問題はなし。
						auto cmd = std::make_unique<ReparentAndReorderCommand>(draggedActor, desiredParent, toIndex);
						CommandManager::Execute(std::move(cmd));
					}
					else
					{
						// ----------------------------------------------------------------
						// 【ケースB】中央にドロップ（＝ターゲットの「直下（子供）」にする）
						// ----------------------------------------------------------------
						size_t toIndex = actor->GetRectTransform()->GetChildActorList().size(); // 子供リストの末尾

						auto cmd = std::make_unique<ReparentAndReorderCommand>(draggedActor, actor, toIndex);
						CommandManager::Execute(std::move(cmd));
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		//ノードが開かれた場合、子オブジェクトを再帰的に描画
		if (open)
		{
			for (UIActorObject* childTransform : children)
			{
				if (UIActorObject* childActor = dynamic_cast<UIActorObject*>(childTransform))
				{
					DrawUIActorNode(childActor);
				}
			}

			//子要素の描画が終了したら
			if (!(node_flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			{
				ImGui::TreePop();
			}
		}
	}

	if (pushedColor)
	{
		ImGui::PopStyleColor();
	}

	ImGui::PopID();
}

bool HierarchyPanel::RightClickMenu()
{
	SetPopupColorTheme();
	//ImGui::BeginPopupContextWindow() は、現在のウィンドウがフォーカスされている状態で
	// 右クリックされた場合にポップアップメニューを開始します。
	if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiMouseButton_Right))
	{
		if (ImGui::MenuItem("Create Empty Actor"))
		{
			auto cmd = std::make_unique<CreateNewActorCommand>();
			CommandManager::Execute(std::move(cmd));
			EditorSettingsManager::SetRenameInputBuffer(SelectionManager::GetSelectedActor()->GetName());
			EditorSettingsManager::SetRenaming(true);
		}
		if (ImGui::MenuItem("Create Empty Canvas")) {
			auto cmd = std::make_unique<CreateNewCanvasCommand>();
			CommandManager::Execute(std::move(cmd));
			EditorSettingsManager::SetRenameInputBuffer(SelectionManager::GetSelectedActor()->GetName());
			EditorSettingsManager::SetRenaming(true);
		}
		if (ImGui::MenuItem("Create Empty UIActor"))
		{
			auto cmd = std::make_unique<CreateNewUIActorCommand>();
			CommandManager::Execute(std::move(cmd));
			EditorSettingsManager::SetRenameInputBuffer(SelectionManager::GetSelectedActor()->GetName());
			EditorSettingsManager::SetRenaming(true);
		}
		if (SelectionManager::GetSelectedActor())
		{
			if (ImGui::MenuItem("Rename", "F2"))
			{
				EditorSettingsManager::SetRenameInputBuffer(SelectionManager::GetSelectedActor()->GetName());
				EditorSettingsManager::SetRenaming(true);
			}

			if (ImGui::MenuItem("Delete Actor","Delete"))
			{
				auto cmd = std::make_unique<DeleteCommand>(SelectionManager::GetSelectedActor());
				CommandManager::Execute(std::move(cmd));
			}
		}
		if (ImGui::MenuItem("GUI Initialization of position"))
		{
			isResetLayout = true;
		}
		ImGui::EndPopup();
	}
	ResetPopupColorTheme();
	return true;
}

void HierarchyPanel::ClearPointer()
{
	SelectionManager::SetSelectedActor(nullptr);
}