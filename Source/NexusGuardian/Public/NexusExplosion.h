#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusExplosion.generated.h"

UCLASS()
class NEXUSGUARDIAN_API ANexusExplosion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexusExplosion();
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* DetectionArea;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	class UParticleSystem* ExplosionEffect; // 폭발 이펙트

	UPROPERTY(EditAnywhere, Category = "Explosion")
	class USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionDamage = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionRadius = 300.0f;

	UFUNCTION()
	void OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	bool bHasExploded = false;
protected:
	virtual void BeginPlay() override;
	void ExecuteExplosion(); // 실제 폭발 실행 함수
	FTimerHandle ExplosionTimerHandle;

};
