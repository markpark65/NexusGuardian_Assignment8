#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NexusGuardianAIController.generated.h"

UCLASS()
class NEXUSGUARDIAN_API ANexusGuardianAIController : public AAIController
{
	GENERATED_BODY()
public:
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float DetectionRange = 1000.0f;
};
