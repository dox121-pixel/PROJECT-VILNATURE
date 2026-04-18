// VilNatureCharacter.cpp
#include "Character/VilNatureCharacter.h"

#include "Character/FlightComponent.h"
#include "Combat/CombatComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "VFX/VFXManager.h"

AVilNatureCharacter::AVilNatureCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Spring arm — gives nice third-person distance
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength        = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity    = 700.0f;
    GetCharacterMovement()->AirControl       = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed     = 600.0f;
    GetCharacterMovement()->MaxFlySpeed      = 5000.0f;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw   = false;
    bUseControllerRotationRoll  = false;

    // Gameplay components
    FlightComp = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
    CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
}

void AVilNatureCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    // Register Enhanced Input mapping context
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

void AVilNatureCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Hold jump to activate flight
    if (!bIsFlightMode && GetCharacterMovement()->IsFalling())
    {
        JumpHoldTimer += DeltaTime;
        if (JumpHoldTimer >= FlightActivationHoldTime)
        {
            bIsFlightMode = true;
            FlightComp->StartFlight();
        }
    }
}

// ─── Input setup ──────────────────────────────────────────────────────────────

void AVilNatureCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EIC) return;

    EIC->BindAction(IA_Move,        ETriggerEvent::Triggered, this, &AVilNatureCharacter::Move);
    EIC->BindAction(IA_Look,        ETriggerEvent::Triggered, this, &AVilNatureCharacter::Look);
    EIC->BindAction(IA_Jump,        ETriggerEvent::Started,   this, &AVilNatureCharacter::OnJumpPressed);
    EIC->BindAction(IA_Jump,        ETriggerEvent::Completed, this, &AVilNatureCharacter::OnJumpReleased);
    EIC->BindAction(IA_FlyUp,       ETriggerEvent::Triggered, this, &AVilNatureCharacter::OnFlyUp);
    EIC->BindAction(IA_Boost,       ETriggerEvent::Started,   this, &AVilNatureCharacter::OnBoostPressed);
    EIC->BindAction(IA_Boost,       ETriggerEvent::Completed, this, &AVilNatureCharacter::OnBoostReleased);
    EIC->BindAction(IA_LightAttack, ETriggerEvent::Started,   this, &AVilNatureCharacter::OnLightAttack);
    EIC->BindAction(IA_HeavyAttack, ETriggerEvent::Started,   this, &AVilNatureCharacter::OnHeavyAttack);
    EIC->BindAction(IA_EyeLaser,    ETriggerEvent::Started,   this, &AVilNatureCharacter::OnEyeLaserStarted);
    EIC->BindAction(IA_EyeLaser,    ETriggerEvent::Completed, this, &AVilNatureCharacter::OnEyeLaserCompleted);
    EIC->BindAction(IA_GroundSlam,  ETriggerEvent::Started,   this, &AVilNatureCharacter::OnGroundSlam);
}

// ─── Input handlers ───────────────────────────────────────────────────────────

void AVilNatureCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller && !Axis.IsNearlyZero())
    {
        const FRotator Rot   = Controller->GetControlRotation();
        const FRotator YawRot(0.f, Rot.Yaw, 0.f);
        const FVector  Fwd   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        const FVector  Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

        if (bIsFlightMode)
        {
            FlightComp->AddFlightInput(Fwd,  Axis.Y);
            FlightComp->AddFlightInput(Right, Axis.X);
        }
        else
        {
            AddMovementInput(Fwd,  Axis.Y);
            AddMovementInput(Right, Axis.X);
        }
    }
}

void AVilNatureCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}

void AVilNatureCharacter::OnJumpPressed()
{
    JumpHoldTimer = 0.0f;

    if (bIsFlightMode)
    {
        FlightComp->AddFlightInput(FVector::UpVector, 1.0f);
    }
    else
    {
        Jump();
    }
}

void AVilNatureCharacter::OnJumpReleased()
{
    StopJumping();

    if (bIsFlightMode)
    {
        FlightComp->StopFlight();
        bIsFlightMode = false;
    }

    JumpHoldTimer = 0.0f;
}

void AVilNatureCharacter::OnFlyUp(const FInputActionValue& Value)
{
    if (bIsFlightMode)
    {
        FlightComp->AddFlightInput(FVector::UpVector, Value.Get<float>());
    }
}

void AVilNatureCharacter::OnBoostPressed()
{
    FlightComp->SetBoostActive(true);
}

void AVilNatureCharacter::OnBoostReleased()
{
    FlightComp->SetBoostActive(false);
}

void AVilNatureCharacter::OnLightAttack()
{
    CombatComp->PerformLightAttack();
}

void AVilNatureCharacter::OnHeavyAttack()
{
    CombatComp->PerformHeavyAttack();
}

void AVilNatureCharacter::OnEyeLaserStarted()
{
    CombatComp->SetEyeLaserActive(true);

    // Notify VFX manager
    if (AVFXManager* VFX = Cast<AVFXManager>(
            UGameplayStatics::GetActorOfClass(this, AVFXManager::StaticClass())))
    {
        VFX->SetHeatDistortionActive(true);
    }
}

void AVilNatureCharacter::OnEyeLaserCompleted()
{
    CombatComp->SetEyeLaserActive(false);

    if (AVFXManager* VFX = Cast<AVFXManager>(
            UGameplayStatics::GetActorOfClass(this, AVFXManager::StaticClass())))
    {
        VFX->SetHeatDistortionActive(false);
    }
}

void AVilNatureCharacter::OnGroundSlam()
{
    if (bIsFlightMode || GetCharacterMovement()->IsFalling())
    {
        CombatComp->PerformGroundSlam();
    }
}

// ─── Stats ────────────────────────────────────────────────────────────────────

float AVilNatureCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
    const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth = FMath::Clamp(CurrentHealth - Applied, 0.0f, MaxHealth);

    // Trigger blood spatter proportional to damage
    if (IsLocallyControlled())
    {
        if (AVFXManager* VFX = Cast<AVFXManager>(
                UGameplayStatics::GetActorOfClass(this, AVFXManager::StaticClass())))
        {
            VFX->TriggerBloodSpatter(DamageAmount / MaxHealth);
        }
    }

    return Applied;
}

float AVilNatureCharacter::GetHealthFraction() const
{
    return (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
}
