// CombatComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UNiagaraSystem;
class UAnimMontage;
class UPhysicalMaterial;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
    Light   UMETA(DisplayName="Light"),
    Heavy   UMETA(DisplayName="Heavy"),
    EyeLaser UMETA(DisplayName="Eye Laser"),
    GroundSlam UMETA(DisplayName="Ground Slam"),
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
    Front  UMETA(DisplayName="Front"),
    Back   UMETA(DisplayName="Back"),
    Left   UMETA(DisplayName="Left"),
    Right  UMETA(DisplayName="Right"),
    Down   UMETA(DisplayName="Down (Slam)"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackLanded, EAttackType, AttackType, AActor*, HitActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboWindow);

/**
 * CombatComponent
 *
 * Full superhero melee / ranged combat system:
 *   - 4-hit light combo chain with hit-stop (freeze frames)
 *   - Heavy attack breaks combo and applies ragdoll impulse
 *   - Eye laser continuous beam with heat-distortion material toggle
 *   - Ground slam with radial Chaos field force + shockwave FX
 *   - Directional hit reactions on enemies
 *   - Blood / impact decal spawning
 *   - Slow-motion kill-cam on finishing blow
 */
UCLASS(ClassGroup=(VilNature), meta=(BlueprintSpawnableComponent))
class VILNATURE_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    // ── Attack API ────────────────────────────────────────────────────────────

    /** Trigger the next attack in the combo chain. */
    UFUNCTION(BlueprintCallable, Category="Combat")
    void PerformLightAttack();

    /** Heavy attack — clears combo, applies ragdoll impulse to target. */
    UFUNCTION(BlueprintCallable, Category="Combat")
    void PerformHeavyAttack();

    /** Toggle continuous eye-laser beam. */
    UFUNCTION(BlueprintCallable, Category="Combat")
    void SetEyeLaserActive(bool bActive);

    /** Slam downward — spawns ground shockwave and Chaos radial force. */
    UFUNCTION(BlueprintCallable, Category="Combat")
    void PerformGroundSlam();

    /** True while attack animation is playing and input is buffered. */
    UFUNCTION(BlueprintPure, Category="Combat")
    bool IsAttacking() const { return bIsAttacking; }

    // ── Events ────────────────────────────────────────────────────────────────

    UPROPERTY(BlueprintAssignable, Category="Combat")
    FOnAttackLanded OnAttackLanded;

    /** Fired during the combo-window frame so Blueprints can chain the next hit. */
    UPROPERTY(BlueprintAssignable, Category="Combat")
    FOnComboWindow OnComboWindowOpen;

    // ── Tuning ────────────────────────────────────────────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Melee")
    float LightAttackReach  = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Melee")
    float HeavyAttackReach  = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Melee")
    float LightAttackDamage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Melee")
    float HeavyAttackDamage = 100.0f;

    /** Duration of the freeze-frame hit-stop effect in seconds. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Feel",
              meta=(ClampMin="0.0", ClampMax="0.2"))
    float HitStopDuration   = 0.06f;

    /** Impulse magnitude applied to ragdolled enemies on heavy hit. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Feel")
    float RagdollImpulse    = 80000.0f;

    /** Radius of the ground-slam shockwave. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|GroundSlam")
    float SlamRadius        = 600.0f;

    /** Force magnitude of the Chaos radial field on slam. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|GroundSlam")
    float SlamChaosForce    = 500000.0f;

    // ── VFX references ────────────────────────────────────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|VFX")
    TObjectPtr<UNiagaraSystem> ImpactFlashFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|VFX")
    TObjectPtr<UNiagaraSystem> ShockwaveRingFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|VFX")
    TObjectPtr<UNiagaraSystem> BloodSplatterFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|VFX")
    TObjectPtr<UNiagaraSystem> EyeLaserFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|VFX")
    TObjectPtr<UNiagaraSystem> GroundSlamDustFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Anim")
    TArray<TObjectPtr<UAnimMontage>> LightAttackMontages;  // 4-hit chain

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Anim")
    TObjectPtr<UAnimMontage> HeavyAttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Anim")
    TObjectPtr<UAnimMontage> GroundSlamMontage;

protected:
    virtual void BeginPlay() override;

private:
    bool  bIsAttacking     = false;
    bool  bEyeLaserActive  = false;
    int32 ComboIndex       = 0;         // 0-3 light combo steps
    float HitStopTimer     = 0.0f;

    void ExecuteAttackTrace(EAttackType Type, float Reach, float Damage);
    void ApplyHitStop();
    void ApplyRagdollImpulse(AActor* Target, FVector Origin);
    void SpawnGroundSlamField();
    EHitDirection CalculateHitDirection(const FVector& HitterLoc, const FVector& VictimLoc,
                                        const FVector& VictimForward) const;
    void TriggerKillCam(AActor* Target);
};
