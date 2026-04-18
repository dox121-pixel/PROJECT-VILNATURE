// CombatComponent.cpp
#include "Combat/CombatComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "PhysicsField/PhysicsFieldComponent.h"
#include "Field/FieldSystemObjects.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();
}

// ─── Tick — manages hit-stop timer ───────────────────────────────────────────

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (HitStopTimer > 0.0f)
    {
        HitStopTimer -= DeltaTime;
        if (HitStopTimer <= 0.0f)
        {
            HitStopTimer = 0.0f;
            // Resume normal global time dilation
            UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
        }
    }
}

// ─── Public API ───────────────────────────────────────────────────────────────

void UCombatComponent::PerformLightAttack()
{
    if (bIsAttacking) return;

    // Play the correct montage in the 4-hit chain
    ACharacter* Owner = Cast<ACharacter>(GetOwner());
    if (Owner && LightAttackMontages.IsValidIndex(ComboIndex))
    {
        Owner->PlayAnimMontage(LightAttackMontages[ComboIndex]);
    }

    ExecuteAttackTrace(EAttackType::Light, LightAttackReach, LightAttackDamage);

    ComboIndex = (ComboIndex + 1) % FMath::Max(LightAttackMontages.Num(), 1);
    bIsAttacking = true;

    // Notify Blueprints that the combo window is open
    OnComboWindowOpen.Broadcast();

    // Auto-clear attacking flag after montage completes (approximated by delay)
    if (UWorld* World = GetWorld())
    {
        FTimerHandle TempHandle;
        World->GetTimerManager().SetTimer(TempHandle, [this]()
        {
            bIsAttacking = false;
        }, 0.5f, false);
    }
}

void UCombatComponent::PerformHeavyAttack()
{
    if (bIsAttacking) return;

    ACharacter* Owner = Cast<ACharacter>(GetOwner());
    if (Owner && HeavyAttackMontage)
    {
        Owner->PlayAnimMontage(HeavyAttackMontage);
    }

    ComboIndex   = 0;    // Heavy attack resets combo
    bIsAttacking = true;
    ExecuteAttackTrace(EAttackType::Heavy, HeavyAttackReach, HeavyAttackDamage);

    if (UWorld* World = GetWorld())
    {
        FTimerHandle TempHandle;
        World->GetTimerManager().SetTimer(TempHandle, [this]()
        {
            bIsAttacking = false;
        }, 0.8f, false);
    }
}

void UCombatComponent::SetEyeLaserActive(bool bActive)
{
    bEyeLaserActive = bActive;

    AActor* Owner = GetOwner();
    if (!Owner || !EyeLaserFX) return;

    if (bActive)
    {
        // Continuous beam — attached to eye socket via Blueprints/ABP
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            EyeLaserFX,
            Owner->GetRootComponent(),
            FName("EyeSocket_R"),
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTargetIncludingScale,
            true
        );
    }
}

void UCombatComponent::PerformGroundSlam()
{
    if (bIsAttacking) return;

    ACharacter* Owner = Cast<ACharacter>(GetOwner());
    if (Owner && GroundSlamMontage)
    {
        Owner->PlayAnimMontage(GroundSlamMontage);
    }

    SpawnGroundSlamField();

    if (GroundSlamDustFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this,
            GroundSlamDustFX,
            GetOwner()->GetActorLocation(),
            FRotator::ZeroRotator
        );
    }

    if (ShockwaveRingFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this,
            ShockwaveRingFX,
            GetOwner()->GetActorLocation(),
            FRotator::ZeroRotator
        );
    }

    bIsAttacking = true;
    if (UWorld* World = GetWorld())
    {
        FTimerHandle TempHandle;
        World->GetTimerManager().SetTimer(TempHandle, [this]()
        {
            bIsAttacking = false;
        }, 1.2f, false);
    }
}

// ─── Private ──────────────────────────────────────────────────────────────────

void UCombatComponent::ExecuteAttackTrace(EAttackType Type, float Reach, float Damage)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    const FVector Start  = Owner->GetActorLocation();
    const FVector End    = Start + Owner->GetActorForwardVector() * Reach;

    TArray<FHitResult> Hits;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    const bool bHit = GetWorld()->SweepMultiByChannel(
        Hits,
        Start, End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(80.0f),
        Params
    );

    for (const FHitResult& Hit : Hits)
    {
        AActor* Target = Hit.GetActor();
        if (!Target) continue;

        // Apply damage
        UGameplayStatics::ApplyPointDamage(Target, Damage, (End - Start).GetSafeNormal(),
                                           Hit, Owner->GetInstigatorController(),
                                           Owner, nullptr);

        // Hit stop
        ApplyHitStop();

        // Impact flash
        if (ImpactFlashFX)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                this, ImpactFlashFX, Hit.ImpactPoint,
                Hit.ImpactNormal.Rotation());
        }

        // Blood
        if (BloodSplatterFX)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                this, BloodSplatterFX, Hit.ImpactPoint,
                Hit.ImpactNormal.Rotation());
        }

        // Ragdoll on heavy attack
        if (Type == EAttackType::Heavy)
        {
            ApplyRagdollImpulse(Target, Start);
            TriggerKillCam(Target);
        }

        OnAttackLanded.Broadcast(Type, Target);
    }
}

void UCombatComponent::ApplyHitStop()
{
    // Freeze the game for a single freeze-frame duration then resume
    UGameplayStatics::SetGlobalTimeDilation(this, 0.05f);
    HitStopTimer = HitStopDuration;
}

void UCombatComponent::ApplyRagdollImpulse(AActor* Target, FVector Origin)
{
    if (!Target) return;
    if (USkeletalMeshComponent* Mesh = Target->FindComponentByClass<USkeletalMeshComponent>())
    {
        Mesh->SetSimulatePhysics(true);
        const FVector Direction = (Target->GetActorLocation() - Origin).GetSafeNormal();
        Mesh->AddImpulse(Direction * RagdollImpulse, NAME_None, true);
    }
}

void UCombatComponent::SpawnGroundSlamField()
{
    // Spawn a Chaos radial force field to fracture geometry in slam radius
    UWorld* World = GetWorld();
    if (!World) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;

    // Use a transient AFieldSystemActor to inject a radial force
    // (full Chaos field setup is done in Blueprint child class for artist control)
    const FVector SlamLoc = Owner->GetActorLocation();
    TArray<FHitResult> SlamHits;
    UKismetSystemLibrary::SphereTraceMulti(
        World,
        SlamLoc, SlamLoc,
        SlamRadius,
        UEngineTypes::ConvertToTraceType(ECC_WorldDynamic),  // destructible actors use WorldDynamic
        false,
        TArray<AActor*>{Owner},
        EDrawDebugTrace::None,
        SlamHits,
        true
    );

    for (const FHitResult& SH : SlamHits)
    {
        if (UPrimitiveComponent* Prim = SH.GetComponent())
        {
            const FVector Dir = (SH.ImpactPoint - SlamLoc).GetSafeNormal()
                                + FVector(0, 0, 0.5f);  // outward + upward
            Prim->AddImpulseAtLocation(Dir * SlamChaosForce, SH.ImpactPoint);
        }
    }
}

EHitDirection UCombatComponent::CalculateHitDirection(const FVector& HitterLoc,
                                                       const FVector& VictimLoc,
                                                       const FVector& VictimForward) const
{
    const FVector ToHitter = (HitterLoc - VictimLoc).GetSafeNormal2D();
    const float   Dot      = FVector::DotProduct(VictimForward.GetSafeNormal2D(), ToHitter);
    const float   Cross    = FVector::CrossProduct(VictimForward.GetSafeNormal2D(), ToHitter).Z;

    if (Dot > 0.7f)  return EHitDirection::Front;
    if (Dot < -0.7f) return EHitDirection::Back;
    return (Cross > 0.0f) ? EHitDirection::Right : EHitDirection::Left;
}

void UCombatComponent::TriggerKillCam(AActor* Target)
{
    // Slow-motion kill-cam: 0.2x time dilation for 2 perceived seconds.
    // The timer uses wall-clock time, so we multiply by the dilation factor:
    // wall_clock_seconds = perceived_seconds * time_dilation = 2.0f * 0.2f = 0.4f
    static constexpr float KillCamTimeDilation     = 0.2f;
    static constexpr float KillCamPerceivedSeconds = 2.0f;
    static constexpr float KillCamWallClockSeconds = KillCamPerceivedSeconds * KillCamTimeDilation;

    UGameplayStatics::SetGlobalTimeDilation(this, KillCamTimeDilation);

    if (UWorld* World = GetWorld())
    {
        FTimerHandle KillCamHandle;
        World->GetTimerManager().SetTimer(KillCamHandle, [this]()
        {
            UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
        }, KillCamWallClockSeconds, false);
    }
}
