#pragma once
#include "Typedefs.h"
#include <memory>
#include "Entity.h"

// クリップボードのイメージ（EditorSystem内などで保持）
class EditorClipboard
{
private:
    static uint64_t mCopiedActorBuffer; // コピーされたアクターの雛形
	static bool mIsCutOperation; // カット操作かどうかのフラグ
public:

    static void Cut(Entity* target)
    {
        if (!target) return;
        // 【重要】ポインタのコピーを複製して保持する
        mCopiedActorBuffer = target->GetID();
        // ここで、実際のコピー処理（例: コマンドの実行など）を行うこともできます。
        // 例えば、Copyコマンドを作成してCommandManagerに渡すなど。
		mIsCutOperation = true; // カット操作であることを記録
	}

    static void Copy(Entity* target)
    {
        if (!target) return;

        // 【重要】ポインタのコピーを複製して保持する
        mCopiedActorBuffer = target->GetID();
		mIsCutOperation = false; // コピー操作であることを記録
    }

    static uint64_t GetCopiedActor() { return mCopiedActorBuffer; }
    static bool HasCopiedActor() { return mCopiedActorBuffer != 0; }

	// カット操作の場合、ペースト後にクリップボードをクリアする
    static void ClearClipboard()
    {
        mCopiedActorBuffer = 0;
        mIsCutOperation = false;
	}
	static bool IsCutOperation() { return mIsCutOperation; }
};