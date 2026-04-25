#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusActor.generated.h"

UCLASS()
class NEXUSGUARDIAN_API ANexusActor : public AActor
{
	GENERATED_BODY()
	
public:
    ANexusActor();

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    class UWidgetComponent* HealthWidgetComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 500.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHP;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void BeginPlay() override;
};
