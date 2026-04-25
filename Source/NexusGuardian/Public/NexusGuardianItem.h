#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusGuardianItem.generated.h"

UENUM(BlueprintType)
enum class EItemEffect : uint8
{
	Slowing,    // 디버프: 속도 감소
	Reverse,    // 디버프: 조작 반전
	Blind,      // 디버프: 시야 제한
	PowerUp,    // 버프: 공격력 2배
	Heal        // 버프: 체력 회복
};

UCLASS()
class NEXUSGUARDIAN_API ANexusGuardianItem : public AActor
{
	GENERATED_BODY()
	
public:
	ANexusGuardianItem();

    void ApplyRandomEffect(class ANexusGuardianCharacter* Player);
    void RespawnItem();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class USphereComponent* CollisionComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UStaticMeshComponent* ItemMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UWidgetComponent* OverHeadWidget;

    UPROPERTY(EditAnywhere, Category = "ItemSettings")
    float Duration = 5.0f;
};
