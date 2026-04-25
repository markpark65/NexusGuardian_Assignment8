#include "MonsterSpawner.h"
#include "NexusGuardianEnemy.h"
#include "NexusGuardianGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMesh"));
   
    RootComponent = MeshComp;
    
    HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthWidgetComp->SetupAttachment(RootComponent);
    HealthWidgetComp->SetRelativeLocation(FVector(0, 0, 100)); // 머리 위 높이
    HealthWidgetComp->SetDrawSize(FVector2D(80.f, 10.f));
    HealthWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
    
    CurrentHP = MaxHP;
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();

    CurrentHP = MaxHP;

    if (HealthWidgetComp)
    {
        UUserWidget* HealthWidget = HealthWidgetComp->GetUserWidgetObject();
        if (HealthWidget)
        {
            FObjectProperty* TargetActorProp = FindFieldChecked<FObjectProperty>(HealthWidget->GetClass(), FName("TargetActor"));

            if (TargetActorProp)
            {
                TargetActorProp->SetObjectPropertyValue_InContainer(HealthWidget, this);
            }
        }
    }

    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AMonsterSpawner::SpawnMonster, SpawnInterval, true);
}

void AMonsterSpawner::InitWaveData(int32 InMaxCount)
{
    MaxSpawnCount = InMaxCount;
    CurrentSpawnCount = 0;
    bIsActive = true;
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AMonsterSpawner::SpawnMonster, SpawnInterval, true);
    UE_LOG(LogTemp, Warning, TEXT("스폰너 초기화: 이번 웨이브 총 %d마리"), MaxSpawnCount);
}

void AMonsterSpawner::SpawnMonster()
{
    if (!bIsActive || CurrentSpawnCount >= MaxSpawnCount)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

        if (auto GM = Cast<ANexusGuardianGameMode>(GetWorld()->GetAuthGameMode())) GM->CheckWaveClear();
        return;
    }

    if (EnemyClass)
    {
        ANexusGuardianGameMode* GM = Cast<ANexusGuardianGameMode>(GetWorld()->GetAuthGameMode());
        if (!GM) return;

        if (CurrentSpawnCount >= MaxSpawnCount)
        {
            GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
            GM->CheckWaveClear();
            return;
        }
        
        FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.0f;
        FRotator SpawnRotation = GetActorRotation();

        // 보스 웨이브 보스 우선 소환
        if (GM && GM->IsBossWave() && !bBossSpawned)
        {
            if (GM->BossEnemyClass)
            {
                GetWorld()->SpawnActor<ANexusGuardianEnemy>(GM->BossEnemyClass, SpawnLocation, SpawnRotation);
                bBossSpawned = true;
                CurrentSpawnCount++;
                return;
            }
        }

        // 일반 적 소환
        ANexusGuardianEnemy* NewEnemy = GetWorld()->SpawnActor<ANexusGuardianEnemy>(EnemyClass, SpawnLocation, SpawnRotation);
        if (NewEnemy)
        {
            if (GM)
            {
                float HPMul = GM->GetMonsterHealthMultiplier();
                float DmgMul = GM->GetMonsterDamageMultiplier();

                NewEnemy->ApplyStatMultiplier(HPMul, DmgMul);
            }
            CurrentSpawnCount++;
        }
    }
}

float AMonsterSpawner::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHP -= ActualDamage;

    if (CurrentHP <= 0)
    {
        if (auto GM = Cast<ANexusGuardianGameMode>(GetWorld()->GetAuthGameMode()))
        {
            GM->OnSpawnerDestroyed();
        }
        Destroy();
    }

    return ActualDamage;
}