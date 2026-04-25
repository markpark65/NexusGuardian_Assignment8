#include "NexusGuardianItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "NexusGuardianCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ANexusGuardianItem::ANexusGuardianItem()
{
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(600.0f);
    CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = CollisionComp;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemMesh->SetupAttachment(RootComponent);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("3DWidget"));
    OverHeadWidget->SetupAttachment(RootComponent);
    OverHeadWidget->SetRelativeLocation(FVector(0, 0, 80.0f));
    OverHeadWidget->SetWidgetSpace(EWidgetSpace::Screen);

}

void ANexusGuardianItem::BeginPlay()
{
	Super::BeginPlay();
    if (CollisionComp)
    {
        CollisionComp->OnComponentBeginOverlap.RemoveDynamic(this, &ANexusGuardianItem::OnOverlapBegin);
        CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ANexusGuardianItem::OnOverlapBegin);

        CollisionComp->OnComponentEndOverlap.RemoveDynamic(this, &ANexusGuardianItem::OnOverlapEnd);
        CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ANexusGuardianItem::OnOverlapEnd);
    }
}

void ANexusGuardianItem::ApplyRandomEffect(ANexusGuardianCharacter* Player)
{
    if (!Player) return;
    int32 RandomIndex = FMath::RandRange(0, 4);
    EItemEffect SelectedEffect = static_cast<EItemEffect>(RandomIndex);

    FTimerManager& TimerManager = GetWorld()->GetTimerManager();

    switch (SelectedEffect)
    {
    case EItemEffect::Slowing:
        Player->GetCharacterMovement()->MaxWalkSpeed *= 0.5f;
        TimerManager.SetTimer(Player->ItemTimerHandle, [Player]() {
            if (Player) Player->GetCharacterMovement()->MaxWalkSpeed = Player->WalkSpeed;
            }, Duration, false);
        break;

    case EItemEffect::Reverse:
        Player->bIsReversed = true;
        TimerManager.SetTimer(Player->ItemTimerHandle, [Player]() {
            if (Player) Player->bIsReversed = false;
            }, Duration, false);
        break;

    case EItemEffect::PowerUp:
        Player->AttackDamage *= 2.0f;
        TimerManager.SetTimer(Player->ItemTimerHandle, [Player]() {
            if (Player) Player->AttackDamage /= 2.0f; // 다시 절반으로 원복
            }, Duration, false);
        break;

    case EItemEffect::Heal:
        // 즉시 회복 (회복은 지속시간 없음)
        Player->CurrentHP = FMath::Min(Player->MaxHP, Player->CurrentHP + 30.0f);
        break;

    case EItemEffect::Blind:
        Player->bIsBlinded = true;
        TimerManager.SetTimer(Player->ItemTimerHandle, [Player]() {
            if (Player) Player->bIsBlinded = false;
            }, Duration, false);
        break;
    }

    Player->OnItemPurchased(SelectedEffect, Duration);

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}
void ANexusGuardianItem::RespawnItem()
{
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}
void ANexusGuardianItem::PlayFadeInAnim()
{
    ReceivePlayFadeIn();
}

void ANexusGuardianItem::PlayFadeOutAnim()
{
    ReceivePlayFadeOut();
}
void ANexusGuardianItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (Cast<ANexusGuardianCharacter>(OtherActor))
    {
        PlayFadeInAnim();
    }
}

void ANexusGuardianItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (Cast<ANexusGuardianCharacter>(OtherActor))
    {
        PlayFadeOutAnim();
    }
}