#pragma once
#include "Typedefs.h"
#include <memory>
#include "Actor.h"

// クリップボードのイメージ（EditorSystem内などで保持）
class EditorClipboard
{
private:
    static ActorObject* mCopiedActorBuffer; // コピーされたアクターの雛形

public:
    static void Copy(ActorObject* target)
    {
        if (!target) return;

        // 【重要】ポインタのコピーを複製して保持する
        mCopiedActorBuffer = target;
    }

    static ActorObject* GetCopiedActor() { return mCopiedActorBuffer; }
    static bool HasCopiedActor() { return mCopiedActorBuffer != nullptr; }
};

