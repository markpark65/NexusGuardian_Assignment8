#include "NexusGuardianPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NexusGuardianCharacter.h"

ANexusGuardianPlayerController::ANexusGuardianPlayerController()
    : InputMappingContext(nullptr)
    , MoveAction(nullptr)
    , JumpAction(nullptr)
    , LookAction(nullptr)
    , SprintAction(nullptr)
    , AttackAction(nullptr)
    , PickUpAction(nullptr)
{
}
void ANexusGuardianPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer()) {

        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem <UEnhancedInputLocalPlayerSubsystem>()) {
            if (InputMappingContext) {
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }
}



void ANexusGuardianPlayerController::SetUIInputMode(bool bIsUI)
{
    if (bIsUI)
    {
        FInputModeGameAndUI InputMode;
        SetInputMode(InputMode);
        bShowMouseCursor = true;
    }
    else
    {
        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
        bShowMouseCursor = false;
    }
}