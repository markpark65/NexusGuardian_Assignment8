#include "NexusGuardianEnemy.h"
#include "NexusGuardianCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NexusGuardianGameMode.h"
#include "AIController.h"
#include "Components/WidgetComponent.h"
ANexusGuardianEnemy::ANexusGuardianEnemy()
{
    PrimaryActorTick.bCanEverTick = false;
    MaxHP = 50.f;
    CurrentHP = MaxHP;
    AttackRange = 300.f;
    AttackDamage = 10.f;
    AttackSpeed = 1.5f;
    ExperienceValue = 20.f;

    HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthWidgetComp->SetupAttachment(RootComponent);
    HealthWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
    HealthWidgetComp->SetDrawSize(FVector2D(80.f, 10.f));
    HealthWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
}

void ANexusGuardianEnemy::BeginPlay()
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
    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ANexusGuardianEnemy::AttemptAttack, AttackSpeed, true);
}
void ANexusGuardianEnemy::ApplyStatMultiplier(float HPMultiplier, float DamageMultiplier)
{
    MaxHP *= HPMultiplier;
    CurrentHP = MaxHP;

    AttackDamage *= DamageMultiplier;

    UE_LOG(LogTemp, Warning, TEXT("적 강화 완료! HP: %f, Damage: %f"), MaxHP, AttackDamage);
}
float ANexusGuardianEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHP -= ActualDamage;

    if (CurrentHP <= 0)
    {
        ANexusGuardianCharacter* Player = Cast<ANexusGuardianCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
        if (Player) Player->GainExp(30.0f);
        ANexusGuardianGameMode* GM = Cast<ANexusGuardianGameMode>(GetWorld()->GetAuthGameMode());

        Destroy();
        if (GM)
        {
            GM->CheckWaveClear();
        }

    }
    return ActualDamage;
}

void ANexusGuardianEnemy::AttemptAttack()
{
    AAIController* AICon = Cast<AAIController>(GetController());
    if (!AICon) return;

    AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    TArray<AActor*> FoundNexuses;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Nexus"), FoundNexuses);
    AActor* NexusActor = (FoundNexuses.Num() > 0) ? FoundNexuses[0] : nullptr;

    AActor* BestTarget = nullptr;

    if (PlayerPawn && GetDistanceTo(PlayerPawn) <= AttackRange)
    {
        BestTarget = PlayerPawn;
    }
    else if (NexusActor && GetDistanceTo(NexusActor) <= AttackRange)
    {
        BestTarget = NexusActor;
    }

    if (BestTarget)
    {
        float Distance = GetDistanceTo(BestTarget);
        if (Distance <= AttackRange)
        {
            UGameplayStatics::ApplyDamage(
                BestTarget,
                AttackDamage,
                AICon,
                this,
                UDamageType::StaticClass()
            );
        }
    }
}

void ANexusGuardianEnemy::OnDeath()
{
    ANexusGuardianCharacter* Player = Cast<ANexusGuardianCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Player)
    {
        Player->GainExp(ExperienceValue);
    }

    GetWorldTimerManager().ClearTimer(AttackTimerHandle);
    Destroy();
}