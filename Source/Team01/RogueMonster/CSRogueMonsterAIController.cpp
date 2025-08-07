#include "CSRogueMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "CSRogueMonster.h"

ACSRogueMonsterAIController::ACSRogueMonsterAIController()
{
	//Blackboardcomp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTRef(TEXT("'/Game/Team01/RogueMonster/AI/BT_RogueMonster.BT_RogueMonster'"));
	//if (BTRef.Succeeded())
	//{
	//	BehaviorTreeAsset = BTRef.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UBlackboardData> BBRef(TEXT("''/Game/Team01/RogueMonster/AI/BB_RogueMonster.BB_RogueMonster'"));
	//if (BBRef.Succeeded())
	//{
	//	BlackboardAsset = BBRef.Object;
	//}
}

void ACSRogueMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BlackboardAsset, Blackboardcomp))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}

	InitPatrolTarget(InPawn);
}

void ACSRogueMonsterAIController::InitPatrolTarget(APawn* InPawn)
{
	if (!Blackboardcomp || !InPawn) return;

	ACSRogueMonster* Monster = Cast<ACSRogueMonster>(InPawn);
	if (!Monster || Monster->PatrolPoints.Num() == 0) return;

	AActor* NewTarget = Monster->PatrolPoints[FMath::RandRange(0, Monster->PatrolPoints.Num() - 1)];
	Blackboardcomp->SetValueAsObject(TEXT("PatrolTarget"), NewTarget);
}