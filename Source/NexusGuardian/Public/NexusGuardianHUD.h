#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NexusGuardianHUD.generated.h"

UCLASS()
class NEXUSGUARDIAN_API ANexusGuardianHUD : public AHUD
{
	GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> PlayerHUDClass;

    UPROPERTY()
    class UUserWidget* CurrentPlayerHUD;

protected:
    virtual void BeginPlay() override;
};
