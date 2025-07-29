#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CSPlayerAnimInstance.generated.h"

class ACSCharacterBase;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostDead);

UCLASS()
class TEAM01_API UCSPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UFUNCTION()
	void AnimNotify_CheckHit();
	UFUNCTION()
	void AnimNotify_PostDead();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ACSCharacterBase> OwnerCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GroundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bShouldMove : 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsFalling : 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsDead : 1;

	UPROPERTY(BlueprintReadOnly)
	float NormalizedCurrentPitch;

public:
	FOnCheckHit OnCheckHit;
	FOnPostDead OnPostDead;
	
};
