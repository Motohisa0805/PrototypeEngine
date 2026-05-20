#include "Time.h"

float Time::mMaxDeltaTime = 0.05f;

float Time::mFrameRate = 0.0f;

Uint64 Time::mMaxTicksCount = 0;

float Time::gTimeScale = 1.0f;

float Time::gUnscaledDeltaTime = 0.0f;

float Time::gDeltaTime = 0.0f;

Uint64 Time::gTicksCount = 0.0f;

float Time::mAccumTime = 0.0f;

int Time::mFrameCount = 0;

void Time::InitializeDeltaTime()
{
	gTicksCount = SDL_GetTicksNS();
}

void Time::UpdateDeltaTime()
{
	Uint64 targetTicks = gTicksCount + mMaxTicksCount;
	Uint64 currentTicks = SDL_GetTicksNS();

    if (currentTicks < targetTicks) {
        Uint64 waitTime = targetTicks - currentTicks;
		
        if(waitTime > 1'000'000) // 1ms以上待つ必要がある場合はSDL_DelayPreciseを使用
        {
            SDL_DelayNS(waitTime - 500'000); // 5ms前に目覚める
		}
        // 最後の微調整は精度のためにビジーウェイトで行う
        while (SDL_GetTicksNS() < targetTicks);
        currentTicks = SDL_GetTicksNS();
    }

    // unscaledDeltaTimeをまず計算
    gUnscaledDeltaTime = (currentTicks - gTicksCount) / 1'000'000'000.0f;

    // クランプ (大きすぎるdeltaTimeを防ぐ)
    if (gUnscaledDeltaTime > mMaxDeltaTime)
    {
        gUnscaledDeltaTime = mMaxDeltaTime;
    }

    // timeScaleを適用してdeltaTimeを作成
    gDeltaTime = gUnscaledDeltaTime * gTimeScale;

    // 実際の経過時間を蓄積
    mAccumTime += gUnscaledDeltaTime;
    mFrameCount++;
    // 0.5秒（あるいは1.0f）経過したら表示用FPS（mFrameRate）を更新
    if (mAccumTime >= 0.5f) {
        mFrameRate = static_cast<float>(mFrameCount) / mAccumTime;

        mAccumTime = 0.0f;
        mFrameCount = 0;
    }

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
    // 60FPS : 16ms = 16,000,000ns
    // 120FPS : 8.33ms = 8'333'333ns
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
