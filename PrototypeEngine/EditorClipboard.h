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

        // 古いバッファがあれば消す
        if (mCopiedActorBuffer) { delete mCopiedActorBuffer; }

        // 【重要】ポインタのコピーではなく、中身を複製（ディープコピー）して保持する
        mCopiedActorBuffer = target->Clone();
    }

    static ActorObject* GetCopiedActor() { return mCopiedActorBuffer; }
    static bool HasCopiedActor() { return mCopiedActorBuffer != nullptr; }
};

