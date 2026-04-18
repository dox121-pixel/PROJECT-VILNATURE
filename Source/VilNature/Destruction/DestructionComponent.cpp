// DestructionComponent.cpp
#include "Destruction/DestructionComponent.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/DecalComponent.h"
#include "DrawDebugHelpers.h"

UDestructionComponent::UDestructionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDestructionComponent::BeginPlay()
{
    Super::BeginPlay();
    CacheGeometryCollection();
}

// ─── Public API ───────────────────────────────────────────────────────────────

void UDestructionComponent::ApplyDestructionForce(FVector WorldLocation, float Magnitude,
                                                   float Radius)
{
    if (Magnitude < FractureThreshold) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // Only act if the impact is close enough to this actor
    const float DistSq = FVector::DistSquared(WorldLocation, Owner->GetActorLocation());
    if (DistSq > FMath::Square(Radius * 2.0f)) return;

    bFractured = true;

    if (GeoCollection)
    {
        // Apply an impulse at the impact point to break internal Chaos joints near it.
        // AddImpulseAtLocation is inherited from UPrimitiveComponent and works with
        // the Chaos solver; more fine-grained per-fragment fracturing can be wired up
        // via an AFieldSystemActor in a Blueprint child class.
        GeoCollection->AddImpulseAtLocation(
            (Owner->GetActorLocation() - WorldLocation).GetSafeNormal() * Magnitude,
            WorldLocation
        );
    }

    SpawnDestructionVFX(WorldLocation);
    SpawnCraterDecal(WorldLocation);
}

void UDestructionComponent::OnSonicBoomNearby(FVector BoomOrigin, float BoomMagnitude)
{
    ApplyDestructionForce(BoomOrigin, BoomMagnitude * SonicBoomForceMultiplier,
                          1500.0f   /* generous blast radius for sonic boom */);
}

// ─── Private ──────────────────────────────────────────────────────────────────

void UDestructionComponent::CacheGeometryCollection()
{
    GeoCollection = GetOwner()
        ? GetOwner()->FindComponentByClass<UGeometryCollectionComponent>()
        : nullptr;
}

void UDestructionComponent::SpawnDestructionVFX(FVector Location)
{
    if (StructuralDustFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this, StructuralDustFX, Location, FRotator::ZeroRotator);
    }

    if (DebrisShowerFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this, DebrisShowerFX, Location, FRotator::ZeroRotator);
    }
}

void UDestructionComponent::SpawnCraterDecal(FVector Location)
{
    if (!CraterDecalMaterial) return;

    UWorld* World = GetWorld();
    if (!World) return;

    UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
        World,
        CraterDecalMaterial,
        FVector(CraterDecalSize),
        Location,
        FRotator(-90.f, 0.f, 0.f),   // face up
        60.0f                          // lifetime (seconds)
    );

    if (Decal)
    {
        Decal->SetFadeScreenSize(0.001f);
    }
}
