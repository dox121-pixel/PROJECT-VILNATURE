// VilNatureGameMode.cpp
#include "VilNatureGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

AVilNatureGameMode::AVilNatureGameMode()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AVilNatureGameMode::BeginPlay()
{
    Super::BeginPlay();
    ComboCount = 0;
}

int32 AVilNatureGameMode::IncrementCombo()
{
    ++ComboCount;
    ScheduleComboReset();
    BroadcastMusicIntensity();
    return ComboCount;
}

void AVilNatureGameMode::ResetCombo()
{
    ComboCount = 0;
    BroadcastMusicIntensity();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ComboResetTimer);
    }
}

float AVilNatureGameMode::GetComboMultiplier() const
{
    // 1x at 0 hits, caps at 10x at 50+ hits — smooth exponential ramp
    const float Raw = 1.0f + 9.0f * FMath::Min(static_cast<float>(ComboCount) / 50.0f, 1.0f);
    return FMath::RoundToFloat(Raw * 10.0f) / 10.0f;
}

// ─── Private ──────────────────────────────────────────────────────────────────

void AVilNatureGameMode::ScheduleComboReset()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            ComboResetTimer,
            this,
            &AVilNatureGameMode::ResetCombo,
            ComboResetDelay,
            false
        );
    }
}

void AVilNatureGameMode::BroadcastMusicIntensity() const
{
    const float Intensity = FMath::Min(static_cast<float>(ComboCount) / 50.0f, 1.0f);
    OnMusicIntensityChanged.Broadcast(Intensity);
}
