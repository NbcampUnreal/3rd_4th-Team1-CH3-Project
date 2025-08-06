#include "CSMummyMonsterBTDecorator.h"
#include "AIController.h"
#include "CSMummyMonster.h"
#include "Team01/Character/CSCharacterBase.h"

UCSMummyMonsterBTDecorator::UCSMummyMonsterBTDecorator()
{
	NodeName = TEXT("Check Monster Condition");
}

bool UCSMummyMonsterBTDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return false;

	ACSMummyMonster* Monster = Cast<ACSMummyMonster>(ControlledPawn);
	if (!Monster) return false;

	switch (ConditionType)
	{
	case EMummyMonsterAIConditionType::IsDead:
		return Monster->bIsDead;

	case EMummyMonsterAIConditionType::IsAttacking:
		return Monster->bIsAttack;

	case EMummyMonsterAIConditionType::IsHit:
		return Monster->bIsHit;

	case EMummyMonsterAIConditionType::IsDetectedPlayer:
		return Monster->bIsDetectedPlayer;

	default:
		return false;
	}
}