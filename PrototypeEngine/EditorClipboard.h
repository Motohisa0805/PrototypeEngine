#pragma once
#include "Typedefs.h"
#include <memory>
#include "Entity.h"

// クリップボードのイメージ（EditorSystem内などで保持）
class EditorClipboard
{
private:
    static uint64_t mCopiedActorBuffer; // コピーされたアクターの雛形

public:
    static void Copy(Entity* target)
    {
        if (!target) return;

        // 【重要】ポインタのコピーを複製して保持する
        mCopiedActorBuffer = target->GetID();
    }

    static uint64_t GetCopiedActor() { return mCopiedActorBuffer; }
    static bool HasCopiedActor() { return mCopiedActorBuffer != 0; }
};

