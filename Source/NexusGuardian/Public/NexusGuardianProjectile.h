#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusGuardianProjectile.generated.h"

UCLASS()
class NEXUSGUARDIAN_API ANexusGuardianProjectile : public AActor
{
    GENERATED_BODY()

public:
    ANexusGuardianProjectile();

    UPROPERTY(EditAnywhere, Category = "Damage")
    float Damage = 20.0f;
protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Projectile")
    class UStaticMeshComponent* ProjectileMesh;

    UPROPERTY(VisibleAnywhere, Category = "Movement")
    class UProjectileMovementComponent* ProjectileMovement;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
