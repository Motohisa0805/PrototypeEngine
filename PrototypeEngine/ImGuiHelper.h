#pragma once
#include "Typedefs.h"
#include "Actor.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"


namespace ImGuiHelper
{
	void FragTextButton(const char* lable,const ImVec2 &size,bool &frag);

    bool IsAncestorOf(ActorObject* potentialAncestor, ActorObject* target);

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

