// VilNatureCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VilNatureCharacter.generated.h"

class UFlightComponent;
class UCombatComponent;
class UNPCDialogueComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * AVilNatureCharacter
 *
 * Base superhero character for PROJECT-VILNATURE.
 * Composes FlightComponent + CombatComponent for full superhero feel.
 * Both player and AI-controlled enemies derive from this class.
 */
UCLASS(BlueprintType, Blueprintable)
class VILNATURE_API AVilNatureCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AVilNatureCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    // ── Components ────────────────────────────────────────────────────────────

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VilNature|Components",
              meta=(AllowPrivateAccess=true))
    TObjectPtr<UFlightComponent> FlightComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VilNature|Components",
              meta=(AllowPrivateAccess=true))
    TObjectPtr<UCombatComponent> CombatComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VilNature|Components",
              meta=(AllowPrivateAccess=true))
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VilNature|Components",
              meta=(AllowPrivateAccess=true))
    TObjectPtr<UCameraComponent> FollowCamera;

    // ── Enhanced Input Assets (assign in Blueprint) ───────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_Move;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_Look;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_Jump;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_FlyUp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_Boost;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_LightAttack;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_HeavyAttack;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_EyeLaser;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VilNature|Input")
    TObjectPtr<UInputAction> IA_GroundSlam;

    // ── Stats ─────────────────────────────────────────────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VilNature|Stats")
    float MaxHealth = 1000.0f;

    UPROPERTY(BlueprintReadOnly, Category="VilNature|Stats")
    float CurrentHealth;

    UFUNCTION(BlueprintCallable, Category="VilNature|Stats")
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
                             AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintPure, Category="VilNature|Stats")
    float GetHealthFraction() const;

protected:
    virtual void BeginPlay() override;

private:
    // Input handlers
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void OnJumpPressed();
    void OnJumpReleased();
    void OnFlyUp(const FInputActionValue& Value);
    void OnBoostPressed();
    void OnBoostReleased();
    void OnLightAttack();
    void OnHeavyAttack();
    void OnEyeLaserStarted();
    void OnEyeLaserCompleted();
    void OnGroundSlam();

    bool bIsFlightMode = false;
    float JumpHoldTimer = 0.0f;
    static constexpr float FlightActivationHoldTime = 0.3f;
};
