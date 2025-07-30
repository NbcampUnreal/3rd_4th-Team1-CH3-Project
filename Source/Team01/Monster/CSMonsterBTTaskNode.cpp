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

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	float Distance = FVector::Dist(Monster->GetActorLocation(), Player->GetActorLocation());

	if (Distance < Monster->AttackRange)
	{
		Monster->BeginAttack();
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}