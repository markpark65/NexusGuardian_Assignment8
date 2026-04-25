#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusGuardianEnemy.generated.h"

UCLASS()
class NEXUSGUARDIAN_API ANexusGuardianEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ANexusGuardianEnemy();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 50.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHP;
    void ApplyStatMultiplier(float HPMultiplier, float DamageMultiplier);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackDamage = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float AttackRange = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float AttackSpeed = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float ExperienceValue = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    bool bIsBoss = false;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    class UWidgetComponent* HealthWidgetComp;
protected:
	virtual void BeginPlay() override;
    void AttemptAttack();
    void OnDeath();

    FTimerHandle AttackTimerHandle;

};
