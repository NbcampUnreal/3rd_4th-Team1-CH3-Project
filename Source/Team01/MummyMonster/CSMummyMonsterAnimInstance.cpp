#include "CSMummyMonsterAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "CSMummyMonster.h"
#include "CSMummyMonsterBTDecorator.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCSMummyMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	OwnerPawn = TryGetPawnOwner();
	if (!OwnerPawn) return;

	Speed = OwnerPawn->GetVelocity().Size();

	ACSMummyMonster* Monster = Cast<ACSMummyMonster>(OwnerPawn);
	if (Monster)
	{
		bIsDead = Monster->bIsDead;
	}
}