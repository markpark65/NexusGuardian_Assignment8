#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NexusGuardianGameMode.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FWaveConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 MonsterCount;

    UPROPERTY(EditAnywhere, Category = "Wave")
    bool bIsBossWave;
};

USTRUCT(BlueprintType)
struct FStageConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Stage")
    TArray<FWaveConfig> Waves;

    UPROPERTY(EditAnywhere, Category = "Stage")
    float MonsterDamageMultiplier;

    UPROPERTY(EditAnywhere, Category = "Stage")
    float HealthMultiplier = 1.5f;
};

UCLASS()
class NEXUSGUARDIAN_API ANexusGuardianGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    ANexusGuardianGameMode();

    void OnNexusDestroyed();
    void OnSpawnerDestroyed();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundBase* MainBGM;
    void CheckWaveClear();

protected:
    virtual void BeginPlay() override;

    void StartStage();
    void StartWave();
    void EndGame(bool bIsVictory);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameData")
    TArray<FStageConfig> Stages;

    UPROPERTY(EditAnywhere, Category = "Stages|Mechanics")
    TSubclassOf<class ANexusExplosion> ExplosionClass;

    FTimerHandle ExplosionSpawnTimerHandle;

    void SpawnRandomExplosion();

    int32 CurrentStageIndex;
    int32 CurrentWaveIndex;

public:
    UFUNCTION(BlueprintCallable)
    int32 GetDisplayStage() const { return CurrentStageIndex + 1; }

    UFUNCTION(BlueprintCallable)
    int32 GetDisplayWave() const { return CurrentWaveIndex + 1; }

    UFUNCTION(BlueprintPure, Category = "GameData")
    float GetNexusHPRatio() const;

    UFUNCTION(BlueprintPure, Category = "GameData")
    int32 GetCurrentStage() const { return CurrentStageIndex + 1; }

    UFUNCTION(BlueprintPure, Category = "GameData")
    int32 GetCurrentWave() const { return CurrentWaveIndex + 1; }

    UPROPERTY(EditAnywhere, Category = "GameData")
    TSubclassOf<class AMonsterSpawner> SpawnerClass;
    
    UFUNCTION(BlueprintPure, Category = "GameData")
    bool IsBossWave() const;

    float GetMonsterHealthMultiplier() const;
    float GetMonsterDamageMultiplier() const;

    UPROPERTY(EditAnywhere, Category = "GameData")
    TSubclassOf<class ANexusGuardianEnemy> BossEnemyClass;

    void ResetPlayerPosition();

    UPROPERTY(EditAnywhere, Category = "GameData")
    FVector SpawnerSpawnLocation;
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> GameWinWidgetClass;
};
