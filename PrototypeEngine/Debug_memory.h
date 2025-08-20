#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

/*
* Debug memory management macros
*/

//メモリリークチェックを有効にする
inline void EnableMemoryLeakCheck()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}
//メモリリークチェックを無効にする
inline void BreakOnAllocation(int allocNumber)
{
    _CrtSetBreakAlloc(allocNumber);
}

#ifndef DISABLE_DEBUG_NEW
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif