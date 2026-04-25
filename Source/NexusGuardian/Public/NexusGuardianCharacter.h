#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NexusGuardianProjectile.h"
#include "NexusGuardianCharacter.generated.h"

class USoundBase;
class UParticleSystem;

UCLASS()
class NEXUSGUARDIAN_API ANexusGuardianCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANexusGuardianCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* AttackAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* PickUpAction;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    class UStaticMeshComponent* WeaponMesh;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump(const FInputActionValue& Value);
    void StopJump(const FInputActionValue& Value);
    void StartSprint(const FInputActionValue& Value);
    void StopSprint(const FInputActionValue& Value);
    void Attack(const FInputActionValue& Value);
    void PickUp(const FInputActionValue& Value);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurrentHP;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackDamage = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float WalkSpeed = 600.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float SprintSpeed = 1000.0f;
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float CurrentExp;
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float MaxExp;
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    int32 CurrentLevel;

    
    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetHPRatio() const { return CurrentHP / MaxHP; }

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetExpRatio() const
    {
        if (MaxExp <= 0.0f) return 0.0f;
        return CurrentExp / MaxExp;
    }
    void GainExp(float Amount);
    void LevelUp();
    
    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<ANexusGuardianProjectile> ProjectileClass;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    FVector MuzzleLocation = FVector(0, 0, 0);

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // 발사 소리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    USoundBase* FireSound;

    // 총구 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    UParticleSystem* MuzzleEffect;

    FTimerHandle ItemTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bIsReversed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bIsBlinded = false;

    UFUNCTION(BlueprintImplementableEvent, Category = "Item")
    void OnItemPurchased(EItemEffect EffectType, float Time);
private:
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    class USpringArmComponent* CameraBoom;
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    class UCameraComponent* FollowCamera;
};
