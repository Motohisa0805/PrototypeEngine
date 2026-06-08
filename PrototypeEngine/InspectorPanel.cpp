#include "InspectorPanel.h"
#include "HierarchyPanel.h"//GetSelectedActor()を使うために必要
#include "ComponentFactory.h"
#include "Actor.h"
#include "UIActor.h"
#include "Math.h"

InspectorPanel::InspectorPanel(Renderer* renderer)
	:EditorWindow(renderer)
{
	mID = "Inspector";
}

InspectorPanel::~InspectorPanel()
{

}

void InspectorPanel::Initialize(float width, float height, ImTextureRef ref)
{	
	mWidthPos = width * 0.8f;
	mHeightPos = 55.0f;
	mWidthSize = width * 0.2f;
	mHeightSize = height - 55.0f;
	EditorWindow::Initialize(width, height, ref);
}

void InspectorPanel::Draw(float width, float height)
{
	EditorWindow::Draw(width, height);
	//  新しいウィンドウの作成
	if(ImGui::Begin(GetID().c_str(), &mIsShow, ImGuiWindowFlags_NoCollapse))
	{
		BaseGUIPanelPopupMenu();

		//1.選択中のActorを取得
		Entity* selectedActor = SelectionManager::GetSelectedActor();

		if(selectedActor)
		{
			// ----------------------------------------------------------------
			// 1.Actorの名前を表示・編集
			// ----------------------------------------------------------------
			// C++ std::stringを直接 ImGui::InputText に渡すための処理が必要
			// ここでは簡易的に、Actor::mName を公開メンバーとして扱う

			//Actor名の編集(一時バッファを使うの安全だけど、ここでは簡略化)
			char nameBuffer[128];
			strncpy_s(nameBuffer, selectedActor->GetName().c_str(), sizeof(nameBuffer) - 1);
			nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // 念のためヌル終端
			//アクターの名前入力UI
			if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
			{
				auto cmd = std::make_unique<RenameCommand>(SelectionManager::GetSelectedActor(), string(nameBuffer));
				CommandManager::Execute(std::move(cmd));
			}
			ImGui::SameLine();
			//StaticタグのコンボUI
			if (ImGui::BeginCombo("Static", ActorInformation::GetStateName(selectedActor->GetStatic()).c_str()))
			{
				for (uint32_t i = 0; i < 4; ++i) {
					ActorInformation::StaticTag tag = static_cast<ActorInformation::StaticTag>(i);
					bool isSelected = (selectedActor->GetStatic() == tag);

					if (ImGui::Selectable(ActorInformation::GetStateName(tag).c_str(), isSelected))
					{
						selectedActor->SetStaticTag(tag);
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Separator();

			// ----------------------------------------------------------------
			//2.Transformコンポーネントの表示・編集(ActorObjectはActorObjectの基底クラス)
			// ----------------------------------------------------------------
			bool isStatic = selectedActor->IsStatic() && GUIEditorManager::IsPlaying();
			if (auto actor = dynamic_cast<ActorObject*>(selectedActor)) {
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					// Staticならこれ以降のUI入力を無効化（グレーアウト）する
					ImGui::BeginDisabled(isStatic);

					//DrawTransformProperties(selectedActor);
					actor->GetTransform()->DrawCustomGUI(actor->GetTransform()->GetProperties());

					// 無効化範囲の終了
					ImGui::EndDisabled();

					// 補足：なぜ動かせないかのヒントテキスト出力
					if (isStatic) {
						ImGui::TextDisabled("(?)");
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Static Objects can't be moved while running.");
						}
					}
				}
			}
			else if(auto uiactor = dynamic_cast<UIActorObject*>(selectedActor))
			{
				if (ImGui::CollapsingHeader("RectTransform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					// Staticならこれ以降のUI入力を無効化（グレーアウト）する
					ImGui::BeginDisabled(isStatic);

					//DrawTransformProperties(selectedActor);
					uiactor->GetRectTransform()->DrawCustomGUI(uiactor->GetRectTransform()->GetProperties());

					// 無効化範囲の終了
					ImGui::EndDisabled();

					// 補足：なぜ動かせないかのヒントテキスト出力
					if (isStatic) {
						ImGui::TextDisabled("(?)");
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Static Objects can't be moved while running.");
						}
					}
				}
			}

			//----------------------------------------------------------------
			// コンポーネントのリスト表示
			//----------------------------------------------------------------
			const vector<Component*>& components = selectedActor->GetComponents();

			// 削除対象のコンポーネントを保持するポインタ
			Component* compToDelete = nullptr;
			//コンポーネントラベルにユニークIDを追加するためのインデックス
			int index = 0;
			for (Component* comp : components)
			{
				comp->InitializeDrawCustomGUI();
				// 各コンポーネントのプロパティ編集UI
				if (ImGui::CollapsingHeader((comp->GetName() + std::to_string(index)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					//ここに各コンポーネント固有のプロパティ編集ロジックを実装

					// --------------------------------------------------
					// 【重要】リフレクション情報に基づいてプロパティを描画
					// --------------------------------------------------
					comp->DrawCustomGUI(comp->GetProperties());

					//----------------------------------------------------------------
					// コンポーネント削除ボタン
					//----------------------------------------------------------------

					ImGui::SameLine();

					ImGui::PushID(comp); // comp のアドレスを一時的にIDスタックに追加
					{
						// 削除ボタンの処理
						if (comp->GetName() != "Transform" && ImGui::Button("Remove"))
						{
							compToDelete = comp;
						}
					}
					ImGui::PopID(); // IDスタックから comp のアドレスを削除

				}
				comp->EndDrawCustomGUI();
				index++;
			}
			ImGui::Separator();

			//ループ処理後に削除を実行
			if (compToDelete)
			{
				selectedActor->RemoveComponent(compToDelete);
				delete compToDelete;
				compToDelete = nullptr;
			}

			// ----------------------------------------------------------------
			// 2. 新規コンポーネントの追加ボタン
			// ----------------------------------------------------------------
			//ウィンドウ全体に広がる大きなボタン
			if (ImGui::Button("Add Component", ImVec2(-1.0f, 0.0f)))
			{
				ImGui::OpenPopup("ComponentSelector");
			}

			if (ImGui::BeginPopup("ComponentSelector"))
			{
				//ファクトリーから登録されたコンポーネント名リストを取得
				vector<string> componentNames = ComponentFactory::GetRegisteredComponentNames();

				for (const string& compName : componentNames)
				{
					//既にActorにアタッチされているコンポーネントは表示しない(Transformは除く)
					if (ImGui::MenuItem(compName.c_str()))
					{
						//ファクトリーを使ってコンポーネントを生成
						Component* newComp = ComponentFactory::CreateComponent(compName, selectedActor);
						if (newComp)
						{
							selectedActor->AddComponent(newComp);
							// メッシュとコライダーの依存性が解決したら処理
							selectedActor->OnComponentAdded(newComp); // ← ActorObject側で実装する
							selectedActor->GetBaseTransform()->SetDirty();
						}
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
		}
		else
		{
			ImGui::Text("Select an Actor in the Hierarchy.");
		}
	}
	ImGui::End();
}

void InspectorPanel::DrawTransformProperties(Entity* transform)
{
	bool isStatic = transform->IsStatic() && GUIEditorManager::IsPlaying();
	// Staticならこれ以降のUI入力を無効化（グレーアウト）する
	ImGui::BeginDisabled(isStatic);

	//Position(Vector3)の編集
	Vector3 pos = transform->GetBaseTransform()->GetLocalPosition();
	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))//0.1fはドラッグの感度
	{
		//ローカル関数なので注意
		transform->GetBaseTransform()->SetLocalPosition(pos);
	}
	//回転だけローカルで取得
	//ローカルならスケール値を含まないため
	Vector3 rot = transform->GetBaseTransform()->GetRotationEditor();
	//度数法で表示・編集
	if (ImGui::DragFloat3("Rotation(deg)", &rot.x, 1.0f))
	{

		// ラジアンに変換して保存
		Quaternion qx = Quaternion::CreateFromAxisAngle(Vector3::UnitX, rot.x);
		Quaternion qy = Quaternion::CreateFromAxisAngle(Vector3::UnitY, rot.y);
		Quaternion qz = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, rot.z);
		Quaternion newRotation = qy * qx * qz; // ZXY順で回転を適用
		transform->GetBaseTransform()->SetLocalRotation(newRotation);
		transform->GetBaseTransform()->SetRotationEditor(rot);
	}

	//Scale(Vector3)の編集
	Vector3 scale = transform->GetBaseTransform()->GetLocalScale();
	if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))//0.1fはドラッグの感度
	{
		//ローカル関数なので注意
		transform->GetBaseTransform()->SetLocalScale(scale);
	}

	// 無効化範囲の終了
	ImGui::EndDisabled();

	// 補足：なぜ動かせないかのヒントテキスト出力
	if (isStatic) {
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Static Objects can't be moved while running.");
		}
	}
}

void InspectorPanel::DrawComponentProperties(Component* comp, const PropertyInfo& prop)
{
	//Componentインスタンスの先頭アドレス + オフセット = メンバ変数の値を編集する
	char* dataPtr = reinterpret_cast<char*>(comp) + prop.sOffset;

	//型ごとに適切なImGuiウィジェットを選択
	switch (prop.sType)
	{
	case EPropertyType::E_PT_FLOAT:
		ImGui::DragFloat("##float", reinterpret_cast<float*>(dataPtr), 0.1f);
		break;
	case EPropertyType::E_PT_INT:
		ImGui::DragInt("##int", reinterpret_cast<int*>(dataPtr));
		break;
	case EPropertyType::E_PT_BOOL:
		ImGui::Checkbox("##bool", reinterpret_cast<bool*>(dataPtr));
		break;
	case EPropertyType::E_PT_VECTOR3:
	{
		// Vector3はfloat[3]として扱う
		Vector3* vec = reinterpret_cast<Vector3*>(dataPtr);
		ImGui::DragFloat3("##vec3",vec->GetAsFloatPtr(), 0.1f);
	}
		break;
	case EPropertyType::E_PT_COLOR3:
	{
		// Color3もfloat[3]として扱う
		Vector3* color = reinterpret_cast<Vector3*>(dataPtr);
		ImGui::ColorEdit3("##color3", color->GetAsFloatPtr());
	}
		break;
	case EPropertyType::E_PT_STRING:
	{
		//stringの編集(バッファサイズに注意)
		string* str = reinterpret_cast<string*>(dataPtr);
		//ImGuiはchar*バッファを要求するため、一時バッファを用意
		char buffer[256];
		strncpy_s(buffer, str->c_str(), sizeof(buffer) - 1);
		buffer[sizeof(buffer) - 1] = 0;
		if (ImGui::InputText("##string", buffer, sizeof(buffer)))
		{
			*str = buffer;
		}
	}
		break;
	default:
		ImGui::TextDisabled("Unsupported Type");
		break;
	}
	ImGui::PopID();
}
