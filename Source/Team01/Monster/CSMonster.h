#pragma once

#include "CoreMinimal.h"
#include "Team01/Character/CSCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "CSMonster.generated.h"

UCLASS()
class TEAM01_API ACSMonster : public ACSCharacterBase
{
	GENERATED_BODY()

public:	
	ACSMonster();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void BeginAttack() override;
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;

	void ChasePlayer();
	void Die();

	UPROPERTY(BlueprintReadOnly)
	bool bIsDie;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAttack;

	UPROPERTY(EditAnywhere, Category = "Monster|Range")
	float SightRange;

	UPROPERTY(EditAnywhere, Category = "Monster|Range")
	float AttackRange;

protected:
	// 🔴 추가: HP바 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> HPBarComponent;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY()
	APawn* PlayerPawn;
	
};
