#include "CSHiddenMonsterBTTaskNode.h"
#include "AIController.h"
#include "CSHiddenMonster.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCSHiddenMonsterBTTaskNode::UCSHiddenMonsterBTTaskNode()
{
	NodeName = TEXT("Attack Task");
}

EBTNodeResult::Type UCSHiddenMonsterBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACSHiddenMonster* Monster = Cast<ACSHiddenMonster>(AIController->GetPawn());
	if (!Monster) return EBTNodeResult::Failed;

	AIController->StopMovement();

	Monster->bIsAttack = true;

	return EBTNodeResult::Succeeded;
}