#include "CSRogueRangeAttackBTTaskNode.h"
#include "AIController.h"
#include "CSRogueMonster.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UCSRogueRangeAttackBTTaskNode::UCSRogueRangeAttackBTTaskNode()
{
	NodeName = TEXT("Range Attack Task");
}

EBTNodeResult::Type UCSRogueRangeAttackBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACSRogueMonster* Monster = Cast<ACSRogueMonster>(AIController->GetPawn());
	if (!Monster) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	AIController->StopMovement();

	Monster->BeginRangeAttack();


	return EBTNodeResult::Succeeded;
}