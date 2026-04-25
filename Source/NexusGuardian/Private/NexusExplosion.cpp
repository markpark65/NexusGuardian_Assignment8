#include "NexusExplosion.h"
#include "Kismet/GameplayStatics.h"
#include "NexusGuardianCharacter.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"

ANexusExplosion::ANexusExplosion()
{
	PrimaryActorTick.bCanEverTick = false;
    DetectionArea = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionArea"));
    DetectionArea->SetupAttachment(RootComponent);
    DetectionArea->InitSphereRadius(250.0f);
}

void ANexusExplosion::BeginPlay()
{
    Super::BeginPlay(); 
    DetectionArea->OnComponentBeginOverlap.AddDynamic(this, &ANexusExplosion::OnPlayerDetected);
}
void ANexusExplosion::OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
    if (!bHasExploded && Cast<ANexusGuardianCharacter>(OtherActor)) {
        bHasExploded = true;
        GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ANexusExplosion::ExecuteExplosion, 0.5f, false);
    }
}
void ANexusExplosion::ExecuteExplosion()
{
    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
    }

    TArray<AActor*> IgnoreActors;
    UGameplayStatics::ApplyRadialDamage(
        this, ExplosionDamage, GetActorLocation(), ExplosionRadius,
        UDamageType::StaticClass(), IgnoreActors, this, GetInstigatorController(), true
    );

    Destroy();
}
