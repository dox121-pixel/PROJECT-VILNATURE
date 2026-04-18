// FlightComponent.cpp
#include "Character/FlightComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

UFlightComponent::UFlightComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup    = TG_PrePhysics;
}

void UFlightComponent::BeginPlay()
{
    Super::BeginPlay();
}

// ─── Public API ───────────────────────────────────────────────────────────────

void UFlightComponent::StartFlight()
{
    if (bIsFlying) return;
    bIsFlying       = true;
    bSonicBoomFired = false;

    if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
    {
        UCharacterMovementComponent* Move = Owner->GetCharacterMovement();
        Move->SetMovementMode(MOVE_Flying);
        Move->GravityScale = 0.0f;
    }

    SpawnTrailFX();
}

void UFlightComponent::StopFlight()
{
    if (!bIsFlying) return;
    bIsFlying   = false;
    bBoostActive = false;

    if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
    {
        UCharacterMovementComponent* Move = Owner->GetCharacterMovement();
        Move->SetMovementMode(MOVE_Falling);
        Move->GravityScale = 1.0f;
    }

    FlightVelocity  = FVector::ZeroVector;
    AccumulatedInput = FVector::ZeroVector;
    DestroyTrailFX();
}

void UFlightComponent::AddFlightInput(FVector WorldDirection, float Scale)
{
    AccumulatedInput += WorldDirection.GetSafeNormal() * Scale;
}

void UFlightComponent::SetBoostActive(bool bActive)
{
    bBoostActive = bActive;
    if (TrailComponent)
    {
        // Increase trail density during boost
        TrailComponent->SetFloatParameter(FName("SpawnRate"), bActive ? 500.0f : 150.0f);
    }
}

float UFlightComponent::GetSpeedFraction() const
{
    const float EffectiveMax = MaxSpeed * (bBoostActive ? BoostMultiplier : 1.0f);
    return FMath::Clamp(FlightVelocity.Size() / EffectiveMax, 0.0f, 1.0f);
}

// ─── Tick ─────────────────────────────────────────────────────────────────────

void UFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsFlying) return;

    UpdateVelocity(DeltaTime);
    ApplyCameraEffects(DeltaTime);
    TryFireSonicBoom();

    // Clear input accumulation for next frame
    AccumulatedInput = FVector::ZeroVector;
}

// ─── Private ──────────────────────────────────────────────────────────────────

void UFlightComponent::UpdateVelocity(float DeltaTime)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    const float EffectiveMax = MaxSpeed * (bBoostActive ? BoostMultiplier : 1.0f);
    const FVector DesiredVelocity = AccumulatedInput.GetClampedToMaxSize(1.0f) * EffectiveMax;

    // Separate acceleration / deceleration rates for a punchy feel
    const float Rate = (AccumulatedInput.IsNearlyZero()) ? Deceleration : Acceleration;
    FlightVelocity = FMath::VInterpConstantTo(FlightVelocity, DesiredVelocity, DeltaTime, Rate);

    Owner->SetActorLocation(
        Owner->GetActorLocation() + FlightVelocity * DeltaTime,
        true   // sweep — so we still collide with geometry
    );
}

void UFlightComponent::ApplyCameraEffects(float DeltaTime) const
{
    ACharacter* Owner = Cast<ACharacter>(GetOwner());
    if (!Owner) return;

    const float SpeedFrac = GetSpeedFraction();

    // FOV punch proportional to speed
    if (UCameraComponent* Cam = Owner->FindComponentByClass<UCameraComponent>())
    {
        const float TargetFOV = Cam->FieldOfView + MaxSpeedFOVAddition * SpeedFrac;
        Cam->FieldOfView = FMath::FInterpTo(Cam->FieldOfView, TargetFOV, DeltaTime, 5.0f);
    }

    // Camera shake at high speed
    if (SpeedFrac > 0.7f && HighSpeedCameraShake && Owner->IsLocallyControlled())
    {
        UGameplayStatics::PlayWorldCameraShake(
            this,
            HighSpeedCameraShake,
            Owner->GetActorLocation(),
            0.0f, 500.0f,
            SpeedFrac          // scale
        );
    }
}

void UFlightComponent::TryFireSonicBoom()
{
    const float SpeedFrac = GetSpeedFraction();

    // Reset so the boom can retrigger once speed dips below the threshold again
    if (bSonicBoomFired && SpeedFrac < SonicBoomFraction)
    {
        bSonicBoomFired = false;
    }

    if (bSonicBoomFired) return;
    if (SpeedFrac < SonicBoomFraction) return;

    bSonicBoomFired = true;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    if (SonicBoomFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this,
            SonicBoomFX,
            Owner->GetActorLocation(),
            Owner->GetActorRotation()
        );
    }

    OnSonicBoom.Broadcast(Owner->GetActorLocation());
    // bSonicBoomFired remains true; it is reset in TryFireSonicBoom when
    // speed drops back below the threshold (see gate at top of function).
}

void UFlightComponent::SpawnTrailFX()
{
    if (!AfterburnerTrailFX) return;
    AActor* Owner = GetOwner();
    if (!Owner) return;

    TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
        AfterburnerTrailFX,
        Owner->GetRootComponent(),
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTargetIncludingScale,
        true
    );
}

void UFlightComponent::DestroyTrailFX()
{
    if (TrailComponent)
    {
        TrailComponent->Deactivate();
        TrailComponent = nullptr;
    }
}
