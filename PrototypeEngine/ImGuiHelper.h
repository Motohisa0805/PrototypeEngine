#pragma once
#include "Typedefs.h"
#include "Actor.h"
#include "UIActor.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"


namespace ImGuiHelper
{
    bool DragFloatHelper(const char* label, float* value, float speed = 0.1f,
               float min = 0.0f, float max = 0.0f, const char* format = "%.3f",
               ImGuiSliderFlags flags = 0);

    bool TableSliderFloat(const char* label, float* value, float min = 0.0f, float max = 1.0f, const char* format = "%.3f",
                          ImGuiSliderFlags flags = 0);

    bool TableDragFloatHelper(const char* label, float* value, float speed = 0.1f,
               float min = 0.0f, float max = 0.0f,
               const char*      format = "%.3f",
               ImGuiSliderFlags flags  = 0);


    bool TableColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);

    bool TableColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);

    bool TableCheckbox(const char* label, bool* v);

	void FragTextButton(const char* lable,const ImVec2 &size,bool &frag);

    bool IsAncestorOf(ActorObject* potentialAncestor, ActorObject* target);

    bool IsAncestorOf_UIActor(UIActorObject* potentialAncestor, UIActorObject* target);

    template<typename T>
    void ReorderVector(vector<T>& vec, size_t fromIndex, size_t toIndex)
    {
        if (fromIndex >= vec.size() || toIndex > vec.size() || fromIndex == toIndex) return;

        if (fromIndex < toIndex) {
            // Œã‚ë‚É“®‚©‚·ê‡
            std::rotate(vec.begin() + fromIndex, vec.begin() + fromIndex + 1, vec.begin() + toIndex);
        }
        else {
            // ‘O‚É“®‚©‚·ê‡
            std::rotate(vec.begin() + toIndex, vec.begin() + fromIndex, vec.begin() + fromIndex + 1);
        }
    }
};

