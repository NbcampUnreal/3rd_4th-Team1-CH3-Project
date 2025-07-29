#include "../Animation/CSAnimInstance.h"
#include "../CSCharacterBase.h"
#include "../CSPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCSAnimInstance::NativeInitializeAnimation()
{
	OwnerCharacter = nullptr;
	OwnerCharacterMovement = nullptr;

	APawn* OwnerPawn = TryGetPawnOwner();
	if (IsValid(OwnerPawn))
	{
		OwnerCharacter = Cast<ACSCharacterBase>(OwnerPawn);
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
}

void UCSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (IsValid(OwnerCharacter) && IsValid(OwnerCharacterMovement))
	{
		Velocity = OwnerCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);

		float GroundAcceleration =
			UKismetMathLibrary::VSizeXY(OwnerCharacterMovement->GetCurrentAcceleration());

		bool bIsAccelerationNearlyZero = FMath::IsNearlyZero(GroundAcceleration);
		
		bShouldMove = (KINDA_SMALL_NUMBER < GroundSpeed) && !bIsAccelerationNearlyZero;
		bIsFalling = OwnerCharacterMovement->IsFalling();

		bIsDead = OwnerCharacter->GetIsDead();

		if (APlayerController* OwnerPlayerController =
			Cast<APlayerController>(OwnerCharacter->GetController()))
		{
			NormalizedCurrentPitch =
				UKismetMathLibrary::NormalizeAxis(OwnerPlayerController->GetControlRotation().Pitch);
		}
	}
}

void UCSAnimInstance::AnimNotify_CheckHit()
{
	if (OnCheckHit.IsBound())
	{
		OnCheckHit.Broadcast();
	}
}

void UCSAnimInstance::AnimNotify_PostDead()
{
	if (OnPostDead.IsBound())
	{
		OnPostDead.Broadcast();
	}
}
