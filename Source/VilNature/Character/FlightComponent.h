// FlightComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlightComponent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSonicBoomDelegate, FVector, Location);

/**
 * FlightComponent
 *
 * Drives superhero flight for the VilNature character:
 *   - Smooth acceleration / deceleration curves
 *   - Sonic-boom burst at maximum velocity
 *   - Afterburner trail via Niagara
 *   - Atmospheric camera shake at high speed
 *   - Volumetric cloud parting impulse (broadcast to PCG layer)
 */
UCLASS(ClassGroup=(VilNature), meta=(BlueprintSpawnableComponent))
class VILNATURE_API UFlightComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFlightComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    // ── API ───────────────────────────────────────────────────────────────────

    /** Begin flight mode.  Call when the player presses Jump while airborne. */
    UFUNCTION(BlueprintCallable, Category="Flight")
    void StartFlight();

    /** Land / exit flight mode. */
    UFUNCTION(BlueprintCallable, Category="Flight")
    void StopFlight();

    /** Apply flight input from the player controller (called every frame). */
    UFUNCTION(BlueprintCallable, Category="Flight")
    void AddFlightInput(FVector WorldDirection, float Scale = 1.0f);

    /** Engage afterburner boost (hold BoostFlight action). */
    UFUNCTION(BlueprintCallable, Category="Flight")
    void SetBoostActive(bool bActive);

    /** Current speed as a 0–1 fraction of MaxSpeed. */
    UFUNCTION(BlueprintPure, Category="Flight")
    float GetSpeedFraction() const;

    /** True when currently in flight mode. */
    UFUNCTION(BlueprintPure, Category="Flight")
    bool IsFlying() const { return bIsFlying; }

    // ── Events ────────────────────────────────────────────────────────────────

    /** Fired once when the character first hits sonic-boom velocity. */
    UPROPERTY(BlueprintAssignable, Category="Flight")
    FOnSonicBoomDelegate OnSonicBoom;

    // ── Tuning ────────────────────────────────────────────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Speed")
    float MaxSpeed = 5000.0f;          // cm/s (~180 km/h base)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Speed")
    float BoostMultiplier = 4.0f;      // multiplies MaxSpeed during boost

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Speed")
    float Acceleration = 3000.0f;      // cm/s²

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Speed")
    float Deceleration = 2000.0f;      // cm/s²

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Speed")
    float SonicBoomFraction = 0.85f;   // fraction of max speed that triggers boom

    /** Niagara system used for the afterburner trail. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|VFX")
    TObjectPtr<UNiagaraSystem> AfterburnerTrailFX;

    /** Niagara system for the one-shot sonic boom burst. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|VFX")
    TObjectPtr<UNiagaraSystem> SonicBoomFX;

    /** Camera-shake class applied at high speed. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Camera")
    TSubclassOf<UCameraShakeBase> HighSpeedCameraShake;

    /** FOV added to the default camera FOV at max speed (chromatic aberration). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Camera")
    float MaxSpeedFOVAddition = 15.0f;

protected:
    virtual void BeginPlay() override;

private:
    bool  bIsFlying        = false;
    bool  bBoostActive     = false;
    bool  bSonicBoomFired  = false;

    FVector FlightVelocity = FVector::ZeroVector;
    FVector AccumulatedInput = FVector::ZeroVector;

    UPROPERTY()
    TObjectPtr<UNiagaraComponent> TrailComponent;

    void UpdateVelocity(float DeltaTime);
    void ApplyCameraEffects(float DeltaTime) const;
    void TryFireSonicBoom();
    void SpawnTrailFX();
    void DestroyTrailFX();
};
