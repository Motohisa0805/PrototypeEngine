#include "Time.h"

float Time::mMaxDeltaTime = 0.05f;

float Time::mFrameRate = 0.0f;

Uint64 Time::mMaxTicksCount = TimeLayout::MAX_FPS;

float Time::gTimeScale = 1.0f;

float Time::gUnscaledDeltaTime = 0.0f;

float Time::gDeltaTime = 0.0f;

Uint64 Time::gTicksCount = 0.0f;


void Time::InitializeDeltaTime()
{
	gTicksCount = SDL_GetTicksNS();
}

void Time::UpdateDeltaTime()
{
    // 60FPS : 16ms = 16,000,000ns
    // 120FPS : 8.33ms = 8'333'333ns
    while ((SDL_GetTicksNS() - gTicksCount) < mMaxTicksCount)
        ;

    Uint64 currentTicks = SDL_GetTicksNS();

    // unscaledDeltaTimeをまず計算
    gUnscaledDeltaTime = (currentTicks - gTicksCount) / 1'000'000'000.0f;

    // クランプ (大きすぎるdeltaTimeを防ぐ)
    if (gUnscaledDeltaTime > mMaxDeltaTime)
    {
        gUnscaledDeltaTime = mMaxDeltaTime;
    }

    // timeScaleを適用してdeltaTimeを作成
    gDeltaTime = gUnscaledDeltaTime * gTimeScale;

    // フレームレートを計算（こっちはunscaledでもscaledでも好み）
    mFrameRate = 1.0f / gUnscaledDeltaTime;

    // 次のフレーム用に現在の時刻を保存
    gTicksCount = currentTicks;
}

void Time::SetMaxDeltaTime(float time)
{
    float result = Math::Clamp(time, TimeLayout::MIN_DELTATIME, TimeLayout::MAX_DELTATIME);
    mMaxDeltaTime = result;
}

void Time::SetMaxTicksCount(FrameRateType type)
{
    switch (type)
    {
    case Frame_60:
        mMaxTicksCount = TimeLayout::MIN_FPS;
        break;
    case Frame_120:
        mMaxTicksCount = TimeLayout::MAX_FPS;
        break;
    }
}
