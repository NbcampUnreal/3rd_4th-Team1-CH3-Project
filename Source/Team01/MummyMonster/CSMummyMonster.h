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

public:	
	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAttack;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDetectedPlayer;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHit;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Stat")
	float AttackRange;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Stat")
	float SightRange;;

	UPROPERTY(EditInstanceOnly, Category = "Patrol Point")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	virtual void BeginAttack() override;
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY()
	APawn* PlayerPawn;



	
};
