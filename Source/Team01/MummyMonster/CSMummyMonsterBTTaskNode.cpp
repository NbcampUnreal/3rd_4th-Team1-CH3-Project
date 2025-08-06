#include "CSMummyMonsterBTTaskNode.h"
#include "AIController.h"
#include "CSMummyMonster.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCSMummyMonsterBTTaskNode::UCSMummyMonsterBTTaskNode()
{
	NodeName = TEXT("Attack Task");
}

EBTNodeResult::Type UCSMummyMonsterBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACSMummyMonster* Monster = Cast<ACSMummyMonster>(AIController->GetPawn());
	if (!Monster) return EBTNodeResult::Failed;

	AIController->StopMovement();

	Monster->BeginAttack();

	return EBTNodeResult::Succeeded;
}