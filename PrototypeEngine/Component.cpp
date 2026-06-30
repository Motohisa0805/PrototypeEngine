#include "Component.h"
#include "Actor.h"
#include "UIActor.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

Component::Component(Entity* owner, int updateOrder)
    : mOwner(owner)
    , mActor(static_cast<ActorObject*>(owner))
    , mUIActor(static_cast<UIActorObject*>(owner))
    , mUpdateOrder(updateOrder)
    , mGame(owner->GetGame())
    , mName("Component")
    , mHeaderColor(0.20f, 0.20f, 0.20f, 1.00f)
    , mHeaderHoveredColor(0.26f, 0.59f, 0.98f, 0.65f)
    , mHeaderActiveColor(0.26f, 0.59f, 0.98f, 1.00f)
    , mIsRun(true)
{
}

Component::~Component() {}

const vector<PropertyInfo>& Component::GetPropertiesStatic()
{
    // 基底クラス(Component)自身のプロパティリストを返す
    static vector<PropertyInfo> sProperties;
    if (sProperties.empty())
    {
        // Component固有のプロパティをここに登録(現在はなし)
    }
    return sProperties;
}
// GetPropertiesStatic() を返す（無駄な静的変数の再定義を避ける）
const std::vector<PropertyInfo>& Component::GetProperties() const
{
    return GetPropertiesStatic();
}

void Component::FixedUpdate(float deltaTime) {}

void Component::Update(float deltaTime) {}

void Component::InitializeDrawCustomGUI()
{
    ImGui::PushStyleColor(ImGuiCol_Header,
                          ImVec4(mHeaderColor.x, mHeaderColor.y, mHeaderColor.z,
                                 mHeaderColor.w)); // 通常時
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                          ImVec4(mHeaderHoveredColor.x, mHeaderHoveredColor.y,
                                 mHeaderHoveredColor.z,
                                 mHeaderHoveredColor.w)); // ホバー時
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                          ImVec4(mHeaderActiveColor.x, mHeaderActiveColor.y,
                                 mHeaderActiveColor.z,
                                 mHeaderActiveColor.w)); // 展開時
}

void Component::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
    // 基底クラスのプロパティを表示
    ImGui::Text("Component Base Properties");
    ImGui::Separator();

    // 1.このComponentインスタンスが持つプロパティリストを取得
    const auto& props = properties;

    // 2.すべてのプロパティをループ
    for (const auto& prop : props)
    {
        // 3. プロパティの型情報に基づき、適切なImGuiウィジェットを表示
        // String型のプロパティは自動描画から除外する（特殊なGUIが必要なことが多いため）
        if (prop.sType == EPropertyType::E_PT_STRING)
            continue;

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

void Component::EndDrawCustomGUI() { ImGui::PopStyleColor(3); }

void Component::DrawFloatProperty(const PropertyInfo& prop)
{
    // prop.sOffsetとthisポインタを使ってfloat型メンバ変数へのポインタを計算
    float* valuePtr = GET_MEMBER_PTR(this, prop.sOffset, float);

    ImGui::Text(prop.sName.c_str());
    ImGui::SameLine(ImGui::GetWindowWidth() * 0.4f);
    ImGui::PushID(valuePtr); // ポインタを固有IDとして使用

    // ImGui::DragFloat
    // (速度、最小値、最大値はメタデータで拡張する必要があるが、ここでは仮に設定)
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

    // ImGui::DragFloat
    // (速度、最小値、最大値はメタデータで拡張する必要があるが、ここでは仮に設定)
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

    // リフレクション情報を持って全てのプロパティをシリアライズ
    const auto& properties = this->GetProperties();
    for (const auto& prop : properties)
    {
        // Componentオブジェクトの先頭ポインタとオフセットを使って
        // メンバー変数の実態へのポインタを取得する
        const char* memberPtr =
            reinterpret_cast<const char*>(this) + prop.sOffset;
        // prop.sOffsetとthisポインタを使ってメンバ変数へのポインタを計算
        switch (prop.sType)
        {
        case EPropertyType::E_PT_FLOAT:
            // float型の値をポインタ経由で取得し、JSONに保存
            j[prop.sName] = *reinterpret_cast<const float*>(memberPtr);
            break;
        case EPropertyType::E_PT_INT:
            // float型の値をポインタ経由で取得し、JSONに保存
            j[prop.sName] = *reinterpret_cast<const int*>(memberPtr);
            break;
        case EPropertyType::E_PT_BOOL:
            // float型の値をポインタ経由で取得し、JSONに保存
            j[prop.sName] = *reinterpret_cast<const bool*>(memberPtr);
            break;
        case EPropertyType::E_PT_STRING:
            // string型の値をポインタ経由で取得し、JSONに保存
            j[prop.sName] = *reinterpret_cast<const string*>(memberPtr);
            break;
        // ... 他の型も同様に処理 ...
        default:
            break;
        }
    }
}

void Component::Deserialize(const json& j)
{
    mName = j.at("Type").get<string>();

    // リフレクション情報を使って、すべてのプロパティをデシリアライズする
    const auto& properties = this->GetProperties();
    for (const auto& prop : properties)
    {
        // JSONにそのプロパティが存在するか確認
        if (j.contains(prop.sName))
        {
            // Componentオブジェクトの先頭ポインタとオフセットを使って、
            // メンバー変数の実体へのポインタを取得する (constではない)
            char* memberPtr = reinterpret_cast<char*>(this) + prop.sOffset;

            switch (prop.sType)
            {
            case EPropertyType::E_PT_FLOAT:
                // JSONから値を読み出し、ポインタ経由でfloat型メンバー変数に書き込み
                *reinterpret_cast<float*>(memberPtr) =
                    j[prop.sName].get<float>();
                break;
            case EPropertyType::E_PT_INT:
                // JSONから値を読み出し、ポインタ経由でint型メンバー変数に書き込み
                *reinterpret_cast<int*>(memberPtr) = j[prop.sName].get<int>();
                break;
            case EPropertyType::E_PT_BOOL:
                // JSONから値を読み出し、ポインタ経由でbool型メンバー変数に書き込み
                *reinterpret_cast<bool*>(memberPtr) = j[prop.sName].get<bool>();
                break;
            case EPropertyType::E_PT_STRING:
                // JSONから値を読み出し、ポインタ経由でstring型メンバー変数に書き込み
                *reinterpret_cast<string*>(memberPtr) =
                    j[prop.sName].get<string>();
                break;
                // ... 他の型も同様に処理 ...
            default:
                break;
            }
        }
    }
}
