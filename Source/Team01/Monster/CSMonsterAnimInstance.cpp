#include "CSMonsterAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "CSMonster.h"
#include "CSMonsterBTDecorator.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCSMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	OwnerPawn = TryGetPawnOwner();
	if (!OwnerPawn) return;

	Speed = OwnerPawn->GetVelocity().Size();

	ACSMonster* Monster = Cast<ACSMonster>(OwnerPawn);
	if (Monster)
	{
		bIsDead = Monster->bIsDead;
		bIsAttack = Monster->bIsAttack;
		bIsHit = Monster->bIsHit;
	}
}
