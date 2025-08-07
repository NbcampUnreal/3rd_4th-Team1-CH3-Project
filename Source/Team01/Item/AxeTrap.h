#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AxeTrap.generated.h"

UCLASS()
class TEAM01_API AAxeTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	AAxeTrap();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Trap")
	UStaticMeshComponent* AxeMesh;

	UPROPERTY(VisibleAnywhere, Category = "Trap")
	class UBoxComponent* DamageBox;

	float RunningTime = 0.f;
	FRotator InitialRotation;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 10.f;


	UPROPERTY(EditAnywhere, Category = "Damage")
	float InvincibilityDuration = 1.0f;

	bool bCanDamage = true;
	FTimerHandle InvincibilityTimer;

	float TimeOffset = 0.f;
	float RotationSpeed = 2.f;

	void ResetDamage();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
