#pragma once

#include "CoreMinimal.h"
#include "CSCharacterBase.h"
#include "CSHiddenMonster.generated.h"

class UPawnSensingComponent;
class UStaticMeshComponent;

UCLASS()
class TEAM01_API ACSHiddenMonster : public ACSCharacterBase
{
	GENERATED_BODY()

public:
	ACSHiddenMonster();

	virtual void BeginPlay() override;

	void EndAttack();

	void BeginAttack();

	void Die();

	float GetCurrentHP();
	
	UPROPERTY(EditAnywhere, Category = "Status")
	float AttackRange;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	bool bIsAttack;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	bool bIsDead;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY()
	APawn* PlayerPawn;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UStaticMeshComponent* SightCone;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnConeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
