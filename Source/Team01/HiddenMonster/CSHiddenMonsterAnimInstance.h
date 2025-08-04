#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CSHiddenMonsterAnimInstance.generated.h"

UCLASS()
class TEAM01_API UCSHiddenMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Speed;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	bool bIsAttack;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	bool bIsDetected;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	bool bIsDead;


	
};
