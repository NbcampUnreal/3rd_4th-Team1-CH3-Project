#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSRogueMonster.generated.h"

UCLASS()
class TEAM01_API ACSRogueMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ACSRogueMonster();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float HitDamage;

	float CurrentHP;

	float MaxHP;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float AttackRange;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float SightRange;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAttack;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHit;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDetectedPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Point")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	void Die();

	UPROPERTY()
	APawn* PlayerPawn;

	void BeginAttack();

	UFUNCTION()
	void EndAttack();


};
