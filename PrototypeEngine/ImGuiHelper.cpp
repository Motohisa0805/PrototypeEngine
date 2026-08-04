#include "ImGuiHelper.h"

bool ImGuiHelper::DragFloatHelper(const char* label, float* value,
                                     float speed, float min, float max,
                                     const char* format, ImGuiSliderFlags flags)
{
    ImGui::TextUnformatted(label);
    ImGui::SameLine();

    string id = "##" + string(label);
    return ImGui::DragFloat(id.c_str(), value, speed, min, max, format, flags);
}

bool ImGuiHelper::TableDragFloatHelper(const char* label, float* value,
                                       float speed, float min, float max,
                                       const char*      format,
                                       ImGuiSliderFlags flags)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text(label);
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-FLT_MIN);
    return ImGui::DragFloat(("##" + string(label)).c_str(), value, speed, min, max, format, flags);
}

bool ImGuiHelper::TableColorEdit4(const char* label, float col[4],
                                  ImGuiColorEditFlags flags)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text(label);
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-FLT_MIN);
    return ImGui::ColorEdit4(("##" + string(label)).c_str(), col, flags);
}

bool ImGuiHelper::TableColorEdit3(const char* label, float col[3],
                                  ImGuiColorEditFlags flags)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text(label);
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-FLT_MIN);
    return ImGui::ColorEdit3(("##" + string(label)).c_str(), col, flags);
}

void ImGuiHelper::FragTextButton(const char* lable, const ImVec2& size,
                                 bool& frag)
{
    string flag = "Off";
    if (frag)
    {
        flag = "On";
    }
    string buttonText = lable + flag;
    if (ImGui::Button(buttonText.c_str(), ImVec2(0.0f, 0.0f)))
    {
        frag = !frag;
    }
}

bool ImGuiHelper::IsAncestorOf(ActorObject* potentialAncestor,
                               ActorObject* target)
{
    if (!target || !potentialAncestor)
        return false;
    if (target == potentialAncestor)
        return true;

    // target‚Ìe‚ðã‚É‰ˆ‚Á‚Ä‚¢‚­
    ActorObject* current = target->GetTransform()->GetParentActor();
    while (current != nullptr)
    {
        if (current == potentialAncestor)
            return true;
        current = current->GetTransform()->GetParentActor();
    }

    return false;
}

bool ImGuiHelper::IsAncestorOf_UIActor(UIActorObject* potentialAncestor,
                                       UIActorObject* target)
{
    if (!target || !potentialAncestor)
        return false;
    if (target == potentialAncestor)
        return true;

    // target‚Ìe‚ðã‚É‰ˆ‚Á‚Ä‚¢‚­
    UIActorObject* current = target->GetRectTransform()->GetParentActor();
    while (current != nullptr)
    {
        if (current == potentialAncestor)
            return true;
        current = current->GetRectTransform()->GetParentActor();
    }

    return false;
}
