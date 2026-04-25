#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusSpikeTrap.generated.h"

UCLASS()
class NEXUSGUARDIAN_API ANexusSpikeTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ANexusSpikeTrap();

	void SetTrapActive(bool bActive);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* BaseMesh; // 함정 발판

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* SpikeMesh; // 실제 튀어나오는 가시

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* DetectionArea;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* DamageArea; // 대미지 판정 영역

	FTimerHandle SpikeTimerHandle;
	bool bIsTrapEnabled = false;
	bool bIsUp = false;
	
	UFUNCTION()
	void OnDetectionBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ApplySpikeDamage(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void RaiseSpike();
	void LowerSpike();

	FVector LoweredLocation;
	FVector RaisedLocation;
};
