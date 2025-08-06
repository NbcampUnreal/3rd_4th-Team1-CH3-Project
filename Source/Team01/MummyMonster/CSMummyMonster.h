#pragma once

#include "CoreMinimal.h"
#include "Team01/Character/CSCharacterBase.h"
#include "CSMummyMonster.generated.h"


UCLASS()
class TEAM01_API ACSMummyMonster : public ACSCharacterBase
{
	GENERATED_BODY()

public:
	ACSMummyMonster();

protected:
	virtual void BeginPlay() override;
	;
public:	
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float HitDamage;

	float CurrentHP;

	float MaxHP;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(EditInstanceOnly, Category = "Patrol Point")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	void Die();

	UPROPERTY()
	APawn* PlayerPawn;

	UFUNCTION()
	void OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
};
