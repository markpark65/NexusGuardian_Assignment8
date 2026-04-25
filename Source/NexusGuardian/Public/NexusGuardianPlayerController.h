#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NexusGuardianPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
UCLASS()
class NEXUSGUARDIAN_API ANexusGuardianPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ANexusGuardianPlayerController();
	void SetUIInputMode(bool bIsUI);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* PickUpAction;

protected:
	virtual void BeginPlay() override;

};
