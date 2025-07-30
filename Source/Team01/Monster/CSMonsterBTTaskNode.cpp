#include "CSMonsterBTTaskNode.h"
#include "AIController.h"
#include "CSMonster.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCSMonsterBTTaskNode::UCSMonsterBTTaskNode()
{
	NodeName = TEXT("Attack Task");
}

EBTNodeResult::Type UCSMonsterBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACSMonster* Monster = Cast<ACSMonster>(AIController->GetPawn());
	if (!Monster || Monster->bIsAttack) return EBTNodeResult::Failed;

	AIController->StopMovement();

	Monster->BeginAttack();

	return EBTNodeResult::Succeeded;
}