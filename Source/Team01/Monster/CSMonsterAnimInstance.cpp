#include "CSMonsterAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCSMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	OwnerPawn = TryGetPawnOwner();
	if (!OwnerPawn) return;

	//Speed = OnwerPawn->GetVelocity().Size();

	//ACSMonster* Monster = Cast<ACSMonster>(OwnerPawn);
	//if (Monster)
	//{
	//	bIsDead = Monster->bIsDead;
	//	bIsAttack = Monster->bIsAttack;
	//}


}