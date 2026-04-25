#include "NexusSpikeTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "NexusGuardianCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ANexusSpikeTrap::ANexusSpikeTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

	SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
	SpikeMesh->SetupAttachment(RootComponent);

	DetectionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionArea"));
	DetectionArea->SetupAttachment(RootComponent);
	DetectionArea->SetBoxExtent(FVector(150.f, 150.f, 100.f));

	DamageArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageArea"));
	DamageArea->SetupAttachment(SpikeMesh); // 가시를 따라 움직이도록 가시에 붙임
	DamageArea->SetBoxExtent(FVector(40.f, 40.f, 40.f));

	DamageArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ANexusSpikeTrap::BeginPlay()
{
	Super::BeginPlay();
	LoweredLocation = SpikeMesh->GetRelativeLocation();
	RaisedLocation = LoweredLocation + FVector(0, 0, 70.f);

	DetectionArea->OnComponentBeginOverlap.AddDynamic(this, &ANexusSpikeTrap::OnDetectionBegin);
	DetectionArea->OnComponentEndOverlap.AddDynamic(this, &ANexusSpikeTrap::OnDetectionEnd);
	DamageArea->OnComponentBeginOverlap.AddDynamic(this, &ANexusSpikeTrap::ApplySpikeDamage);

	// 시작할 때는 꺼둠
	SetTrapActive(false);
}

void ANexusSpikeTrap::SetTrapActive(bool bActive)
{
	bIsTrapEnabled = bActive;
	// 비활성화 시 가시를 내림
	if (!bActive) LowerSpike();
}

void ANexusSpikeTrap::OnDetectionBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsTrapEnabled && Cast<ANexusGuardianCharacter>(OtherActor))
	{
		RaiseSpike();
	}
}
void ANexusSpikeTrap::OnDetectionEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ANexusGuardianCharacter>(OtherActor))
	{
		// 플레이어가 나가면 1초 뒤에 내려가게 하거나 즉시 내림
		GetWorldTimerManager().SetTimerForNextTick(this, &ANexusSpikeTrap::LowerSpike);
	}
}

void ANexusSpikeTrap::RaiseSpike()
{
	SpikeMesh->SetRelativeLocation(RaisedLocation);
	bIsUp = true;
	DamageArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ANexusSpikeTrap::LowerSpike()
{
	SpikeMesh->SetRelativeLocation(LoweredLocation);
	bIsUp = false;
	DamageArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void ANexusSpikeTrap::ApplySpikeDamage(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 가시가 올라와 있을 때 닿으면 대미지
	if (bIsUp && Cast<ANexusGuardianCharacter>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, 20.f, nullptr, this, UDamageType::StaticClass());
	}
}



