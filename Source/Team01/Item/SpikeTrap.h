#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;
class USoundBase;

UCLASS()
class TEAM01_API ASpikeTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpikeTrap();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerZone;

	UPROPERTY(EditAnywhere)
	UBoxComponent* SpikeDamageZone;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SpikeMesh;

	UPROPERTY(EditAnywhere, Category = "SpikeTrap|Sound")
	USoundBase* SpikeRiseSound;

	UPROPERTY(EditAnywhere, Category = "SpikeTrap|Movement")
	FVector SpikeOffset = FVector(0.f, 0.f, 300.f);

	UPROPERTY(EditAnywhere, Category = "SpikeTrap|Movement")
	float MoveSpeed = 800.f;

	UPROPERTY(EditAnywhere, Category = "SpikeTrap|Damage")
	float DamageAmount = 30.f;

	FVector StartLocation;
	FVector TargetLocation;
	bool bMovingUp = false;

	FTimerHandle SpikeMoveTimer;

	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void MoveSpike();

	void ApplyDamage();

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

};
