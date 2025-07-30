#include "CSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/CSPlayerAnimInstance.h"

ACSCharacterBase::ACSCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = true;
	

}

void ACSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACSCharacterBase::HandleOnCheckInputAttack()
{
}

void ACSCharacterBase::HandleOnCheckHit()
{
}

void ACSCharacterBase::HandleOnPostDead()
{
}

void ACSCharacterBase::BeginAttack()
{
}

void ACSCharacterBase::EndAttack(UAnimMontage* InMontage, bool bInterrupted)
{
}


