#pragma once

#include "CoreMinimal.h"
#include "CSCharacterBase.h"
#include "../Ui/CS_WBP_EnemyHPBar.h"
#include "Components/WidgetComponent.h"
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

	UFUNCTION()
	void AttackEnd();

	virtual void BeginAttack() override;

	void Die();

	float GetCurrentHP();
	
	UPROPERTY(EditAnywhere, Category = "Status")
	float AttackRange;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	bool bIsAttack;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	bool bIsDetected;

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

	UFUNCTION()
	void OnConeEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

protected:
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> HPBarComponent;

	UPROPERTY()
	TObjectPtr<UCS_WBP_EnemyHPBar> HPBar;

	UPROPERTY()
	AController* LastInstigator = nullptr;
	
	FTimerHandle HPHideTimerHandle;
};
