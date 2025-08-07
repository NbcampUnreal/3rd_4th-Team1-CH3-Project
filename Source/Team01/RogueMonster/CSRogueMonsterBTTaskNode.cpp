#include "CSRogueMonsterBTTaskNode.h"
#include "AIController.h"
#include "CSRogueMonster.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCSRogueMonsterBTTaskNode::UCSRogueMonsterBTTaskNode()
{

}

EBTNodeResult::Type UCSRogueMonsterBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACSRogueMonster* Monster = Cast<ACSRogueMonster>(AIController->GetPawn());
	if (!Monster) return EBTNodeResult::Failed;

	AIController->StopMovement();

	Monster->BeginAttack();

	return EBTNodeResult::Succeeded;
}