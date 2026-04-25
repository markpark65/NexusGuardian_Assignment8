// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class NEXUSGUARDIAN_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:
    AMonsterSpawner();

    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<class ANexusGuardianEnemy> EnemyClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    float SpawnInterval = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 200.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHP;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    
    UPROPERTY(VisibleAnywhere, Category = "UI")
    class UWidgetComponent* HealthWidgetComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaveInfo")
    int32 MaxSpawnCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaveInfo")
    int32 CurrentSpawnCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveInfo")
    bool bIsActive = true;

    void InitWaveData(int32 InMaxCount);
protected:
    virtual void BeginPlay() override;
    void SpawnMonster();
    bool bBossSpawned = false;
    FTimerHandle SpawnTimerHandle;
};
