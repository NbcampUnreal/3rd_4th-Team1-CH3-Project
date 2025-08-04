#include "CSHiddenMonsterBTDecorator.h"
#include "AIController.h"
#include "CSHiddenMonster.h"
#include "Team01/Character/CSCharacterBase.h"

UCSHiddenMonsterBTDecorator::UCSHiddenMonsterBTDecorator()
{
	NodeName = TEXT("Check Monster Condition");
}

bool UCSHiddenMonsterBTDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return false;

	ACSHiddenMonster* Monster = Cast<ACSHiddenMonster>(ControlledPawn);
	if (!Monster) return false;

	switch (ConditionType)
	{
	case EHiddenMonsterAIConditionType::IsDead:
		return Monster->bIsDead;

	case EHiddenMonsterAIConditionType::IsAttacking:
		return Monster->bIsAttack;

	default:
		return false;
	}
}