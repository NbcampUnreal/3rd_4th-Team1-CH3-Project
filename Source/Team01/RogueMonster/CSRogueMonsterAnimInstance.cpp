#include "CSRogueMonsterAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "CSRogueMonster.h"
#include "CSRogueMonsterBTDecorator.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCSRogueMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	OwnerPawn = TryGetPawnOwner();
	if (!OwnerPawn) return;

	Speed = OwnerPawn->GetVelocity().Size();

	ACSRogueMonster* Monster = Cast<ACSRogueMonster>(OwnerPawn);
	if (Monster)
	{
		bIsDead = Monster->bIsDead;
		bIsAttack = Monster->bIsAttack;
		bIsRangeAttack = Monster->bIsRangeAttack;
		bIsHit = Monster->bIsHit;
		bIsOverlap = Monster->bIsOverlap;
	}
}