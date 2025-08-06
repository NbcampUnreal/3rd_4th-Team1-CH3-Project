#include "CSMummyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "CSMummyMonster.h"

ACSMummyAIController::ACSMummyAIController()
{
	Blackboardcomp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTRef(TEXT("'/Game/Team01/MummyMonster/AI/BT_MummyMonster.BT_MummyMonster'"));
	if (BTRef.Succeeded())
	{
		BehaviorTreeAsset = BTRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBRef(TEXT("'/Game/Team01/MummyMonster/AI/BB_MummyMonster.BB_MummyMonster'"));
	if (BBRef.Succeeded())
	{
		BlackboardAsset = BBRef.Object;
	}
}

void ACSMummyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BlackboardAsset, Blackboardcomp))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}

	InitPatrolTarget(InPawn);
}

void ACSMummyAIController::InitPatrolTarget(APawn* InPawn)
{
	if (!Blackboardcomp || !InPawn) return;

	ACSMummyMonster* Monster = Cast<ACSMummyMonster>(InPawn);
	if (!Monster || Monster->PatrolPoints.Num() == 0) return;

	AActor* NewTarget = Monster->PatrolPoints[FMath::RandRange(0, Monster->PatrolPoints.Num() - 1)];
	Blackboardcomp->SetValueAsObject(TEXT("PatrolTarget"), NewTarget);
}