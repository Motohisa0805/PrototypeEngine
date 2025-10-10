#include "InspectorPanel.h"
#include "HierarchyPanel.h"//GetSelectedActor()を使うために必要

#include "Actor.h"
#include "Math.h"//Vector3,Quaternionを使うために必要

// 仮: 外部から HierarchyPanel のインスタンスを取得できる関数があると仮定

InspectorPanel::InspectorPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
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
}

void InspectorPanel::Draw(float width, float height, ImTextureRef ref)
{
	if (isResetLayout)
	{
		// ウインドウ位置とサイズを固定
		ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
		ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
		isResetLayout = false;
	}
	else
	{
		// ウインドウ位置とサイズを固定
		ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
	}
	//  新しいウィンドウの作成
	if(ImGui::Begin("SelectItem", nullptr, ImGuiWindowFlags_NoCollapse))
	{
		GUIPanelMenu();

		//1.選択中のActorを取得
		ActorObject* selectedActor = GUIWinMain::GetHierarchyPanel()->GetSelectedActor();

		if(selectedActor)
		{
			// ----------------------------------------------------------------
			// 1.Actorの名前を表示・編集
			// ----------------------------------------------------------------
			// C++ std::stringを直接 ImGui::InputText に渡すための処理が必要
			// ここでは簡易的に、Actor::mName を公開メンバーとして扱う

			//Actor名の編集(一時バッファを使うの安全だけど、ここでは簡略化)
			char nameBuffer[256];
			strncpy_s(nameBuffer, selectedActor->GetName().c_str(), sizeof(nameBuffer) - 1);
			nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // 念のためヌル終端

			if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
			{
				selectedActor->SetName(string(nameBuffer));
			}

			ImGui::Separator();

			// ----------------------------------------------------------------
			//2.Transformコンポーネントの表示・編集(TransformはActorObjectの基底クラス)
			// ----------------------------------------------------------------
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				//Position(Vector3)の編集
				Vector3 pos = selectedActor->GetPosition();
				if (ImGui::DragFloat3("Position", &pos.x, 0.1f))//0.1fはドラッグの感度
				{
					//TODO:ローカル関数なので注意
					selectedActor->SetPosition(pos);
				}

				//Rotation(Quaternion)の編集
				float rotX = selectedActor->GetRotationAmountX();
				float rotY = selectedActor->GetRotationAmountY();
				float rotZ = selectedActor->GetRotationAmountZ();

				float rotationEuler[3] = { rotX,rotY,rotZ };

				//度数法で表示・編集
				if (ImGui::DragFloat3("Rotation(deg)", rotationEuler, 1.0f))
				{
					// ユーザーがGUIで値を変更した場合、rotationEuler[0]、[1]、[2]が更新されている

					// 更新された値を変数に戻し、ラジアンに変換してActorに設定
					rotX = rotationEuler[0];
					rotY = rotationEuler[1];
					rotZ = rotationEuler[2];

					// ラジアンに変換して保存
					Quaternion qx = Quaternion::CreateFromAxisAngle(Vector3::UnitX, Math::ToRadians(rotX));
					Quaternion qy = Quaternion::CreateFromAxisAngle(Vector3::UnitY, Math::ToRadians(rotY));
					Quaternion qz = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, Math::ToRadians(rotZ));
					Quaternion newRotation = qz * qy * qx; // ZYX順で回転を適用
					selectedActor->SetRotation(newRotation);

					// 回転量を更新
					selectedActor->SetRotationAmountX(rotX);
					selectedActor->SetRotationAmountY(rotY);
					selectedActor->SetRotationAmountZ(rotZ);
				}

				//Scale(Vector3)の編集
				Vector3 scale = selectedActor->GetScale();
				if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))//0.1fはドラッグの感度
				{
					//TODO:ローカル関数なので注意
					selectedActor->SetScale(scale);
				}
			}

			//----------------------------------------------------------------
			// コンポーネントのリスト表示
			//----------------------------------------------------------------
			const vector<Component*>& components = selectedActor->GetComponents();

			// 削除対象のコンポーネントを保持するポインタ
			Component* compToDelete = nullptr;
			for (Component* comp : components)
			{
				
				// 各コンポーネントのプロパティ編集UI
				if (ImGui::CollapsingHeader(comp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					//TODO : ここに各コンポーネント固有のプロパティ編集ロジックを実装

					ImGui::Text("[Type: %s]", comp->GetName().c_str());
					ImGui::SameLine();

					comp->DrawGUI();

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
					// if (selectedActor->GetComponent(compName)) continue; // GetComponent<T>()の文字列版が必要

					if (ImGui::MenuItem(compName.c_str()))
					{
						//ファクトリーを使ってコンポーネントを生成
						Component* newComp = ComponentFactory::CreateComponent(compName, selectedActor);
						if (newComp)
						{
							// メッシュとコライダーの依存性が解決したら処理
							selectedActor->OnComponentAdded(newComp); // ← ActorObject側で実装する
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
