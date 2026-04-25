#include "NexusGuardianAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void ANexusGuardianAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APawn* MyPawn = GetPawn();
    if (!MyPawn) return;

    AActor* BestTarget = nullptr;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        float DistToPlayer = FVector::Dist(MyPawn->GetActorLocation(), PlayerPawn->GetActorLocation());
        // 플레이어 우선 타게팅
        if (DistToPlayer <= 800.0f)
        {
            BestTarget = PlayerPawn;
        }
    }

    // 넥서스 타게팅
    if (!BestTarget)
    {
        TArray<AActor*> FoundNexuses;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Nexus"), FoundNexuses);

        float ClosestDist = MAX_FLT;
        for (AActor* Nexus : FoundNexuses)
        {
            float Dist = FVector::Dist(MyPawn->GetActorLocation(), Nexus->GetActorLocation());
            if (Dist < ClosestDist)
            {
                ClosestDist = Dist;
                BestTarget = Nexus;
            }
        }
    }

    if (BestTarget)
    {
        MoveToActor(BestTarget, 200.0f);
    }
}