#include "CSPlayerAnimInstance.h"
#include "../CSPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCSPlayerAnimInstance::NativeInitializeAnimation()
{
	OwnerCharacter = nullptr;
	OwnerCharacterMovement = nullptr;

	APawn* OwnerPawn = TryGetPawnOwner();
	if (IsValid(OwnerPawn))
	{
		OwnerCharacter = Cast<ACSPlayerCharacter>(OwnerPawn);
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
}

void UCSPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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
		bIsCrouching = OwnerCharacter->GetIsCrouching();
		

		if (APlayerController* OwnerPlayerController =
			Cast<APlayerController>(OwnerCharacter->GetController()))
		{
			NormalizedCurrentPitch =
				UKismetMathLibrary::NormalizeAxis(OwnerPlayerController->GetControlRotation().Pitch);
		}
	}
}

void UCSPlayerAnimInstance::AnimNotify_PostDead()
{
	if (OnPostDead.IsBound())
	{
		OnPostDead.Broadcast();
	}
}
