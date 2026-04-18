// DestructionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DestructionComponent.generated.h"

class UNiagaraSystem;
class UGeometryCollectionComponent;
class URadialForceComponent;
class UMaterialInterface;

/**
 * DestructionComponent
 *
 * Orchestrates Chaos Destruction on buildings / props:
 *   - Registers a GeometryCollectionComponent on the owner
 *   - Applies radial Chaos field forces from punches and sonic booms
 *   - Spawns layered VFX: structural dust, debris shower, crater decal
 *   - Supports LOD-aware enabling (near = full Nanite geo, far = imposter)
 */
UCLASS(ClassGroup=(VilNature), meta=(BlueprintSpawnableComponent))
class VILNATURE_API UDestructionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDestructionComponent();

    // ── API ───────────────────────────────────────────────────────────────────

    /**
     * Apply an explosive radial force at WorldLocation with given Magnitude.
     * Triggers fracture if within range.
     */
    UFUNCTION(BlueprintCallable, Category="Destruction")
    void ApplyDestructionForce(FVector WorldLocation, float Magnitude, float Radius);

    /**
     * Called by the flight system when a sonic boom fires near this actor.
     * Uses SonicBoomForceMultiplier to scale the impact.
     */
    UFUNCTION(BlueprintCallable, Category="Destruction")
    void OnSonicBoomNearby(FVector BoomOrigin, float BoomMagnitude);

    /** True after the building has been partially or fully fractured. */
    UFUNCTION(BlueprintPure, Category="Destruction")
    bool IsFractured() const { return bFractured; }

    // ── Tuning ────────────────────────────────────────────────────────────────

    /** Minimum force required to begin fracturing. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destruction")
    float FractureThreshold = 50000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destruction")
    float SonicBoomForceMultiplier = 2.5f;

    /** Distance at which LOD switches from full Nanite to imposter. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destruction|LOD")
    float ImposterLODDistance = 5000.0f;

    // ── VFX ──────────────────────────────────────────────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destruction|VFX")
    TObjectPtr<UNiagaraSystem> StructuralDustFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destruction|VFX")
    TObjectPtr<UNiagaraSystem> DebrisShowerFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destruction|VFX")
    TObjectPtr<UMaterialInterface> CraterDecalMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Destruction|VFX")
    float CraterDecalSize = 300.0f;

protected:
    virtual void BeginPlay() override;

private:
    bool bFractured = false;

    UPROPERTY()
    TObjectPtr<UGeometryCollectionComponent> GeoCollection;

    void CacheGeometryCollection();
    void SpawnDestructionVFX(FVector Location);
    void SpawnCraterDecal(FVector Location);
};
