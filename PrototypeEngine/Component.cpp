#include "Component.h"
#include "Actor.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

Component::Component(ActorObject* owner, int updateOrder)
	:mOwner(owner)
	, mUpdateOrder(updateOrder)
	, mGame(owner->GetGame())
	, mName("Component")
{
	mOwner->SetDirty();
}

Component::~Component()
{
	mOwner->RemoveComponent(this);
}

const vector<PropertyInfo>& Component::GetPropertiesStatic()
{
	// 基底クラス(Component)自身のプロパティリストを返す
	static vector<PropertyInfo> sProperties;
	if (sProperties.empty()) {
		// Component固有のプロパティをここに登録(現在はなし)
	}
	return sProperties;
}
//GetPropertiesStatic() を返す（無駄な静的変数の再定義を避ける）
const std::vector<PropertyInfo>& Component::GetProperties() const
{
	return GetPropertiesStatic();
}

void Component::FixedUpdate(float deltaTime)
{
}

void Component::Update(float deltaTime)
{
}

void Component::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	//基底クラスのプロパティを表示
	ImGui::Text("Component Base Properties");
	ImGui::Separator();

	//1.このComponentインスタンスが持つプロパティリストを取得
	const auto& props = properties;

	//2.すべてのプロパティをループ
	for (const auto& prop : props)
	{
		// 3. プロパティの型情報に基づき、適切なImGuiウィジェットを表示
		// String型のプロパティは自動描画から除外する（特殊なGUIが必要なことが多いため）
		if (prop.sType == EPropertyType::E_PT_STRING) continue;

		switch (prop.sType)
		{
		case EPropertyType::E_PT_FLOAT:
			DrawFloatProperty(prop);
			break;
		case EPropertyType::E_PT_BOOL:
			DrawBoolProperty(prop);
			break;
		case EPropertyType::E_PT_INT:
			// Intの描画ヘルパーを実装する必要があります
			DrawIntProperty(prop);
			break;
			// ... 他の型も同様に処理 ...
		default:
			break;
		}
	}
}

void Component::DrawFloatProperty(const PropertyInfo& prop)
{
	// prop.sOffsetとthisポインタを使ってfloat型メンバ変数へのポインタを計算
	float* valuePtr = GET_MEMBER_PTR(this, prop.sOffset, float);

	ImGui::Text(prop.sName.c_str());
	ImGui::SameLine(ImGui::GetWindowWidth() * 0.4f);
	ImGui::PushID(valuePtr); // ポインタを固有IDとして使用

	// ImGui::DragFloat (速度、最小値、最大値はメタデータで拡張する必要があるが、ここでは仮に設定)
	ImGui::DragFloat("##value", valuePtr, 0.1f, -FLT_MAX, FLT_MAX);

	ImGui::PopID();
}

void Component::DrawIntProperty(const PropertyInfo& prop)
{
	// prop.sOffsetとthisポインタを使ってfloat型メンバ変数へのポインタを計算
	int* valuePtr = GET_MEMBER_PTR(this, prop.sOffset, int);

	ImGui::Text(prop.sName.c_str());
	ImGui::SameLine(ImGui::GetWindowWidth() * 0.4f);
	ImGui::PushID(valuePtr); // ポインタを固有IDとして使用

	// ImGui::DragFloat (速度、最小値、最大値はメタデータで拡張する必要があるが、ここでは仮に設定)
	ImGui::DragInt("##value", valuePtr, 0.1f, -FLT_MAX, FLT_MAX);

	ImGui::PopID();
}

void Component::DrawBoolProperty(const PropertyInfo& prop)
{
	// prop.sOffsetとthisポインタを使ってbool型メンバ変数へのポインタを計算
	bool* valuePtr = GET_MEMBER_PTR(this, prop.sOffset, bool);

	ImGui::Text(prop.sName.c_str());
	ImGui::SameLine(ImGui::GetWindowWidth() * 0.4f);
	ImGui::PushID(valuePtr); // ポインタを固有IDとして使用

	// ImGui::Checkbox
	ImGui::Checkbox("##value", valuePtr);

	ImGui::PopID();
}

void Component::Serialize(json& j) const
{
	j["Type"] = mName;
	j["UpdateOrder"] = mUpdateOrder;
}

void Component::Deserialize(const json& j)
{
	mName = j.at("Type").get<string>();
	mUpdateOrder = j.at("UpdateOrder").get<int>();
}
