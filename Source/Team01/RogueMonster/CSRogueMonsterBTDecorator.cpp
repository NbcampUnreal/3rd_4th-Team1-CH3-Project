#include "CSRogueMonsterBTDecorator.h"
#include "CSRogueMonsterBTService.h"
#include "AIController.h"
#include "CSRogueMonster.h"
#include "Team01/Character/CSCharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UCSRogueMonsterBTDecorator::UCSRogueMonsterBTDecorator()
{
	NodeName = TEXT("Check Monster Condition");
}

bool UCSRogueMonsterBTDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return false;

	ACSRogueMonster* Monster = Cast<ACSRogueMonster>(ControlledPawn);
	if (!Monster) return false;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return false;

	bool bIsAttackRange = BB->GetValueAsBool(TEXT("IsInAttackRange"));
	bool bInRangedAttackRange = BB->GetValueAsBool(TEXT("IsInRangeAttackRange"));

	switch (ConditionType)
	{
	case ERogueAIConditionType::IsDead:
		return Monster->bIsDead;

	case ERogueAIConditionType::IsAttacking:
		return bIsAttackRange && !bInRangedAttackRange;
	

	case ERogueAIConditionType::IsRangeAttacking:
		return !bIsAttackRange && bInRangedAttackRange;

	case ERogueAIConditionType::IsHit:
		return Monster->bIsHit;

	case ERogueAIConditionType::IsDetectedPlayer:
		return Monster->bIsDetectedPlayer;

	case ERogueAIConditionType::IsOverlap:
		return !Monster->bIsOverlap;

	default:
		return false;
	}
}