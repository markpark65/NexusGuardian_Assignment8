#include "NexusGuardianGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "NexusActor.h"
#include "NexusGuardianEnemy.h"
#include "NexusExplosion.h"
#include "NexusGuardianHUD.h"
#include "Blueprint/UserWidget.h"
#include "NexusGuardianItem.h"
#include "NexusSpikeTrap.h"
#include "MonsterSpawner.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
ANexusGuardianGameMode::ANexusGuardianGameMode()
{
	CurrentStageIndex = 0;
	CurrentWaveIndex = 0;

    HUDClass = ANexusGuardianHUD::StaticClass();
}

void ANexusGuardianGameMode::BeginPlay()
{
	Super::BeginPlay();
    if (MainBGM)
    {
        UGameplayStatics::PlaySound2D(this, MainBGM);
    }
	StartStage();
}
void ANexusGuardianGameMode::ResetPlayerPosition()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
        if (PlayerStart)
        {
            // 위치와 회전값 복구
            PlayerPawn->SetActorLocation(PlayerStart->GetActorLocation());
            PlayerPawn->GetController()->SetControlRotation(PlayerStart->GetActorRotation());

            // 속도 초기화
            PlayerPawn->GetVelocity().ZeroVector;
        }
    }
}
void ANexusGuardianGameMode::StartStage()
{
	CurrentWaveIndex = 0;
	StartWave();
}
bool ANexusGuardianGameMode::IsBossWave() const
{
    if (Stages.IsValidIndex(CurrentStageIndex))
    {
        if (Stages[CurrentStageIndex].Waves.IsValidIndex(CurrentWaveIndex))
        {
            return Stages[CurrentStageIndex].Waves[CurrentWaveIndex].bIsBossWave;
        }
    }
    return false;
}
void ANexusGuardianGameMode::StartWave()
{

    TArray<AActor*> OldEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANexusGuardianEnemy::StaticClass(), OldEnemies);
    for (AActor* Enemy : OldEnemies)
    {
        Enemy->Destroy();
    }

    float CurrentHPMultiplier = 1.0f;
    if (Stages.IsValidIndex(CurrentStageIndex))
    {
        CurrentHPMultiplier = Stages[CurrentStageIndex].HealthMultiplier;
    }
    TArray<AActor*> FoundNexuses;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Nexus"), FoundNexuses);
    for (AActor* NexusActor : FoundNexuses)
    {
        if (ANexusActor* Nexus = Cast<ANexusActor>(NexusActor))
        {
            Nexus->MaxHP *= CurrentHPMultiplier;
            Nexus->CurrentHP = Nexus->MaxHP;
        }
    }

    if (SpawnerClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AMonsterSpawner* Spawner = GetWorld()->SpawnActor<AMonsterSpawner>(SpawnerClass, SpawnerSpawnLocation, FRotator::ZeroRotator);

        if (Spawner && Stages.IsValidIndex(CurrentStageIndex))
        {
            const FStageConfig& CurrentStage = Stages[CurrentStageIndex];
            if (CurrentStage.Waves.IsValidIndex(CurrentWaveIndex))
            {
                int32 CountForThisWave = CurrentStage.Waves[CurrentWaveIndex].MonsterCount;

                Spawner->InitWaveData(CountForThisWave);
            }
        }
    }
    if (Stages.IsValidIndex(CurrentStageIndex))
    {
        if (Stages[CurrentStageIndex].Waves.IsValidIndex(CurrentWaveIndex))
        {
            UE_LOG(LogTemp, Warning, TEXT("Stage %d - Wave %d 시작!"), GetDisplayStage(), GetDisplayWave());

            if (Stages[CurrentStageIndex].Waves[CurrentWaveIndex].bIsBossWave)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WARNING: BOSS APPEARED!"));
            }
        }
    }
    TArray<AActor*> AllTraps;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANexusSpikeTrap::StaticClass(), AllTraps);
    for (AActor* T : AllTraps) Cast<ANexusSpikeTrap>(T)->SetTrapActive(false);
    ANexusGuardianHUD* MyHUD = nullptr;
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        MyHUD = Cast<ANexusGuardianHUD>(PC->GetHUD());
    }
    if (CurrentStageIndex >= 1)
    {
        for (AActor* T : AllTraps) Cast<ANexusSpikeTrap>(T)->SetTrapActive(true);

        if (CurrentWaveIndex == 0 && MyHUD)
        {
            MyHUD->ShowGlobalWarning(TEXT("스테이지 2: 함정이 활성화되었습니다!"));
        }
    }
    if (CurrentStageIndex >= 2)
    {
        if (!GetWorldTimerManager().IsTimerActive(ExplosionSpawnTimerHandle))
        {
            GetWorldTimerManager().SetTimer(ExplosionSpawnTimerHandle, this, &ANexusGuardianGameMode::SpawnRandomExplosion, 3.0f, true);
        }

        if (CurrentWaveIndex == 0 && MyHUD)
        {
            MyHUD->ShowGlobalWarning(TEXT("스테이지 3: 지뢰 구역에 진입했습니다!"));
        }
    }
    else
    {
        // 스테이지 3 미만이면 타이머를 확실히 제거
        GetWorldTimerManager().ClearTimer(ExplosionSpawnTimerHandle);
    }
    if (MyHUD)
    {
        FString WarningMsg = FString::Printf(TEXT("Stage %d - Wave %d 시작!"), CurrentStageIndex + 1, CurrentWaveIndex + 1);
        MyHUD->ShowGlobalWarning(WarningMsg);
    }
}
void ANexusGuardianGameMode::SpawnRandomExplosion()
{
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (!Player || !ExplosionClass) return;

    FVector RandomOffset = FMath::VRand() * FMath::FRandRange(200.f, 600.f);
    RandomOffset.Z = 0;
    FVector SpawnLocation = Player->GetActorLocation() + RandomOffset;

    GetWorld()->SpawnActor<ANexusExplosion>(ExplosionClass, SpawnLocation, FRotator::ZeroRotator);
}

void ANexusGuardianGameMode::OnNexusDestroyed()
{
    EndGame(false);
}

void ANexusGuardianGameMode::OnSpawnerDestroyed()
{
    ResetPlayerPosition();
    CurrentWaveIndex++;
    
    if (CurrentWaveIndex >= Stages[CurrentStageIndex].Waves.Num())
    {
        CurrentStageIndex++;

        if (CurrentStageIndex >= Stages.Num())
        {
            EndGame(true);
        }
        else
        {
            StartStage();
        }
    }
    else
    {
        StartWave();
    }
}

void ANexusGuardianGameMode::EndGame(bool bIsVictory)
{
    UGameplayStatics::SetGamePaused(GetWorld(), true);

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeUIOnly());

        TSubclassOf<UUserWidget> WidgetClass = bIsVictory ? GameWinWidgetClass : GameOverWidgetClass;

        if (WidgetClass)
        {
            UUserWidget* EndWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
            if (EndWidget)
            {
                EndWidget->AddToViewport();
            }
        }
    }
}
float ANexusGuardianGameMode::GetNexusHPRatio() const
{
    TArray<AActor*> FoundNexuses;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Nexus"), FoundNexuses);

    if (FoundNexuses.Num() > 0)
    {
        if (ANexusActor* Nexus = Cast<ANexusActor>(FoundNexuses[0]))
        {
            return Nexus->CurrentHP / Nexus->MaxHP;
        }
    }
    return 0.0f;
}
void ANexusGuardianGameMode::CheckWaveClear()
{
    TArray<AActor*> RemainingEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANexusGuardianEnemy::StaticClass(), RemainingEnemies);

    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), FoundSpawners);

    int32 AliveCount = 0;
    for (AActor* Enemy : RemainingEnemies)
    {
        if (IsValid(Enemy))
        {
            ANexusGuardianEnemy* EnemyActor = Cast<ANexusGuardianEnemy>(Enemy);
            if (EnemyActor && EnemyActor->CurrentHP > 0)
            {
                AliveCount++;
            }
        }
    }
    if (FoundSpawners.Num() == 0 && AliveCount == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Wave Clear Next Wave"));
        TArray<AActor*> AllItems;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANexusGuardianItem::StaticClass(), AllItems);
        for (AActor* ItemActor : AllItems)
        {
            if (ANexusGuardianItem* Item = Cast<ANexusGuardianItem>(ItemActor))
            {
                Item->RespawnItem();
            }
        }
        OnSpawnerDestroyed();
    }
}

float ANexusGuardianGameMode::GetMonsterHealthMultiplier() const
{
    if (Stages.IsValidIndex(CurrentStageIndex))
    {
        return Stages[CurrentStageIndex].HealthMultiplier;
    }
    return 1.0f;
}

float ANexusGuardianGameMode::GetMonsterDamageMultiplier() const
{
    if (Stages.IsValidIndex(CurrentStageIndex))
    {
        return Stages[CurrentStageIndex].MonsterDamageMultiplier;
    }
    return 1.0f;
}