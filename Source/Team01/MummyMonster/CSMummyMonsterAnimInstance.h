#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CSMummyMonsterAnimInstance.generated.h"

UCLASS()
class TEAM01_API UCSMummyMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;



protected:
	UPROPERTY(BlueprintReadOnly, Category = "Monster|Animation")
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = "Monster|Animation")
	bool bIsDead;
	UPROPERTY(BlueprintReadOnly, Category = "Monster|Animation")
	bool bIsAttack;

	UPROPERTY(BlueprintReadOnly, Category = "Monster|Animation")
	bool bIsHit;

	UPROPERTY()
	class APawn* OwnerPawn;
};
