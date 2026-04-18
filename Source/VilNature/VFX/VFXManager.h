// VFXManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VFXManager.generated.h"

class UNiagaraSystem;
class UMaterialParameterCollection;

/**
 * AVFXManager
 *
 * Singleton-style manager actor that owns all global VFX state:
 *   - Registers with the world and exposes static helper functions
 *   - Chromatic aberration post-process control (speed trail)
 *   - Screen-space blood spatter toggle
 *   - Eye-laser heat distortion material parameter push
 *   - Centralized spawn wrappers so Blueprints / C++ call one place
 *
 * Place ONE instance in each persistent level.  Reference via
 * UGameplayStatics::GetActorOfClass().
 */
UCLASS(BlueprintType, Blueprintable)
class VILNATURE_API AVFXManager : public AActor
{
    GENERATED_BODY()

public:
    AVFXManager();

    // ── Post-process parameters ───────────────────────────────────────────────

    /**
     * Drive chromatic aberration intensity from flight speed (0–1).
     * Called every frame by FlightComponent.
     */
    UFUNCTION(BlueprintCallable, Category="VFX|PostProcess")
    void SetSpeedAberration(float SpeedFraction);

    /**
     * Trigger a screen-space blood spatter.
     * @param Intensity  0–1 severity of the hit.
     */
    UFUNCTION(BlueprintCallable, Category="VFX|PostProcess")
    void TriggerBloodSpatter(float Intensity);

    /**
     * Toggle heat-distortion overlay for eye-laser beam.
     */
    UFUNCTION(BlueprintCallable, Category="VFX|PostProcess")
    void SetHeatDistortionActive(bool bActive);

    // ── Niagara helpers ───────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category="VFX|Niagara")
    void SpawnImpactFlash(FVector WorldLocation, FRotator Rotation);

    UFUNCTION(BlueprintCallable, Category="VFX|Niagara")
    void SpawnShockwaveRing(FVector WorldLocation, float RadiusScale = 1.0f);

    UFUNCTION(BlueprintCallable, Category="VFX|Niagara")
    void SpawnSonicBoomBurst(FVector WorldLocation);

    UFUNCTION(BlueprintCallable, Category="VFX|Niagara")
    void SpawnDebrisCloud(FVector WorldLocation);

    // ── Asset References ──────────────────────────────────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX|Assets")
    TObjectPtr<UNiagaraSystem> NS_ImpactFlash;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX|Assets")
    TObjectPtr<UNiagaraSystem> NS_ShockwaveRing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX|Assets")
    TObjectPtr<UNiagaraSystem> NS_SonicBoom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX|Assets")
    TObjectPtr<UNiagaraSystem> NS_DebrisCloud;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX|Assets")
    TObjectPtr<UNiagaraSystem> NS_BloodSpatter;

    /**
     * Material Parameter Collection that drives global post-process values:
     *   - ChromaticAberrationAmount  (float 0-1)
     *   - BloodSpatterAlpha          (float 0-1)
     *   - HeatDistortionStrength     (float 0-1)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX|Assets")
    TObjectPtr<UMaterialParameterCollection> MPC_GlobalVFX;

    // ── Tuning ────────────────────────────────────────────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX|Tuning",
              meta=(ClampMin="0.0", ClampMax="1.0"))
    float MaxChromaticAberration = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX|Tuning")
    float BloodSpatterFadeTime = 3.0f;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    float CurrentBloodAlpha  = 0.0f;
    float BloodFadeTimer     = 0.0f;
    bool  bBloodFading       = false;

    void SetMPCScalar(FName ParameterName, float Value) const;
};
