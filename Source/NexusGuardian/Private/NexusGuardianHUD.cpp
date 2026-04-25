#include "NexusGuardianHUD.h"
#include "Blueprint/UserWidget.h"

void ANexusGuardianHUD::BeginPlay()
{
    Super::BeginPlay();

    if (PlayerHUDClass)
    {
        CurrentPlayerHUD = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
        if (CurrentPlayerHUD)
        {
            CurrentPlayerHUD->AddToViewport();
        }
    }
}
