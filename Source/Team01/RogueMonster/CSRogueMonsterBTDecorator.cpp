#include "CSRogueMonsterBTDecorator.h"
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

	switch (ConditionType)
	{
	case ERogueAIConditionType::IsDead:
		return Monster->bIsDead;

	case ERogueAIConditionType::IsAttack:
		return Monster->bIsAttack;

	case ERogueAIConditionType::IsHit:
		return Monster->bIsHit;

	case ERogueAIConditionType::IsDetectedPlayer:
		return Monster->bIsDetectedPlayer;

	default:
		return false;
	}
}