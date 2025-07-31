#include "CSMonsterBTDecorator.h"
#include "AIController.h"
#include "CSMonster.h"
#include "Team01/Character/CSCharacterBase.h"

UCSMonsterBTDecorator::UCSMonsterBTDecorator()
{
	NodeName = TEXT("Check Monster Condition");
}

bool UCSMonsterBTDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return false;

	ACSMonster* Monster = Cast<ACSMonster>(ControlledPawn);
	if (!Monster) return false;

	switch (ConditionType)
	{
	case EAIConditionType::IsDead:
		return Monster->bIsDead;

	case EAIConditionType::IsNotDead:
		return !Monster->bIsDead;

	case EAIConditionType::IsAttacking:
		return Monster->bIsAttack;

	case EAIConditionType::IsNotAttacking:
		return !Monster->bIsAttack;

	default:
		return false;
	}
}