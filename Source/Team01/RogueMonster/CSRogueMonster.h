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

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Point")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	void Die();

	UPROPERTY()
	APawn* PlayerPawn;


};
