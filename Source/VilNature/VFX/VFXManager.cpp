// VFXManager.cpp
#include "VFX/VFXManager.h"

#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Materials/MaterialParameterCollection.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"

AVFXManager::AVFXManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AVFXManager::BeginPlay()
{
    Super::BeginPlay();

    // Zero all parameters on startup
    SetMPCScalar(FName("ChromaticAberrationAmount"), 0.0f);
    SetMPCScalar(FName("BloodSpatterAlpha"),         0.0f);
    SetMPCScalar(FName("HeatDistortionStrength"),    0.0f);
}

void AVFXManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Fade blood spatter over time
    if (bBloodFading && CurrentBloodAlpha > 0.0f)
    {
        BloodFadeTimer += DeltaTime;
        CurrentBloodAlpha = FMath::Clamp(
            1.0f - (BloodFadeTimer / BloodSpatterFadeTime),
            0.0f, 1.0f
        );
        SetMPCScalar(FName("BloodSpatterAlpha"), CurrentBloodAlpha);

        if (CurrentBloodAlpha <= 0.0f)
        {
            bBloodFading = false;
        }
    }
}

// ─── Post-process ─────────────────────────────────────────────────────────────

void AVFXManager::SetSpeedAberration(float SpeedFraction)
{
    const float AberrationValue = FMath::Lerp(0.0f, MaxChromaticAberration,
                                              FMath::Clamp(SpeedFraction, 0.0f, 1.0f));
    SetMPCScalar(FName("ChromaticAberrationAmount"), AberrationValue);
}

void AVFXManager::TriggerBloodSpatter(float Intensity)
{
    CurrentBloodAlpha = FMath::Clamp(Intensity, 0.0f, 1.0f);
    BloodFadeTimer    = 0.0f;
    bBloodFading      = true;

    SetMPCScalar(FName("BloodSpatterAlpha"), CurrentBloodAlpha);

    if (NS_BloodSpatter)
    {
        // Spawn blood at the player camera location for correct screen-space appearance.
        // Blueprints/child classes can override OnBloodSpatter to attach to the camera.
        APlayerCameraManager* CamMgr = UGameplayStatics::GetPlayerCameraManager(this, 0);
        const FVector SpawnLoc = CamMgr ? CamMgr->GetCameraLocation() : GetActorLocation();
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this, NS_BloodSpatter,
            SpawnLoc,
            FRotator::ZeroRotator
        );
    }
}

void AVFXManager::SetHeatDistortionActive(bool bActive)
{
    SetMPCScalar(FName("HeatDistortionStrength"), bActive ? 1.0f : 0.0f);
}

// ─── Niagara helpers ──────────────────────────────────────────────────────────

void AVFXManager::SpawnImpactFlash(FVector WorldLocation, FRotator Rotation)
{
    if (NS_ImpactFlash)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this, NS_ImpactFlash, WorldLocation, Rotation);
    }
}

void AVFXManager::SpawnShockwaveRing(FVector WorldLocation, float RadiusScale)
{
    if (!NS_ShockwaveRing) return;

    UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        this, NS_ShockwaveRing, WorldLocation, FRotator::ZeroRotator);

    if (Comp)
    {
        Comp->SetFloatParameter(FName("RadiusScale"), RadiusScale);
    }
}

void AVFXManager::SpawnSonicBoomBurst(FVector WorldLocation)
{
    if (NS_SonicBoom)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this, NS_SonicBoom, WorldLocation, FRotator::ZeroRotator);
    }
}

void AVFXManager::SpawnDebrisCloud(FVector WorldLocation)
{
    if (NS_DebrisCloud)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this, NS_DebrisCloud, WorldLocation, FRotator::ZeroRotator);
    }
}

// ─── Private ──────────────────────────────────────────────────────────────────

void AVFXManager::SetMPCScalar(FName ParameterName, float Value) const
{
    if (!MPC_GlobalVFX) return;

    UWorld* World = GetWorld();
    if (!World) return;

    UMaterialParameterCollectionInstance* Instance =
        World->GetParameterCollectionInstance(MPC_GlobalVFX);

    if (Instance)
    {
        Instance->SetScalarParameterValue(ParameterName, Value);
    }
}
