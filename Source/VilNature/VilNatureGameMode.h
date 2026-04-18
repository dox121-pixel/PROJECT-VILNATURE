// VilNatureGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VilNatureGameMode.generated.h"

/**
 * VilNatureGameMode
 *
 * Root game mode for PROJECT-VILNATURE.  Handles:
 *  - Wave / encounter management
 *  - Global combo multiplier tracking
 *  - Dynamic music intensity signalling
 */
UCLASS(BlueprintType, Blueprintable, meta=(ShortTooltip="Root game mode for VilNature"))
class VILNATURE_API AVilNatureGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AVilNatureGameMode();

    // ── Combo / Score ──────────────────────────────────────────────────────────

    /** Increments the global combo counter and returns the new value. */
    UFUNCTION(BlueprintCallable, Category="Combat|Combo")
    int32 IncrementCombo();

    /** Resets the combo counter to zero (called on player hit or idle timeout). */
    UFUNCTION(BlueprintCallable, Category="Combat|Combo")
    void ResetCombo();

    /** Current combo multiplier (1x – 10x). */
    UFUNCTION(BlueprintPure, Category="Combat|Combo")
    float GetComboMultiplier() const;

    /** Current raw combo hit count. */
    UFUNCTION(BlueprintPure, Category="Combat|Combo")
    int32 GetComboCount() const { return ComboCount; }

    // ── Music intensity ────────────────────────────────────────────────────────

    /**
     * Broadcasts the current music intensity level (0.0 – 1.0) so MetaSounds
     * or FMOD can react.  Called automatically whenever the combo changes.
     */
    UPROPERTY(BlueprintAssignable, Category="Audio")
    FOnMusicIntensityChangedDelegate OnMusicIntensityChanged;

protected:
    virtual void BeginPlay() override;

private:
    int32 ComboCount = 0;

    /** Time (seconds) without a hit before the combo resets. */
    UPROPERTY(EditDefaultsOnly, Category="Combat|Combo", meta=(ClampMin="0.5", ClampMax="10.0"))
    float ComboResetDelay = 3.0f;

    FTimerHandle ComboResetTimer;

    void ScheduleComboReset();
    void BroadcastMusicIntensity() const;
};
