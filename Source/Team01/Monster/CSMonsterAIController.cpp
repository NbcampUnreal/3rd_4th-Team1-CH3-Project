#include "CSMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"

ACSMonsterAIController::ACSMonsterAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTRef(TEXT("'/Game/Team01/Monster/AI/BT_CSMonster.BT_CSMonster'"));
	if (BTRef.Succeeded())
	{
		BehaviorTreeAsset = BTRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBRef(TEXT("'/Game/Team01/Monster/AI/BB_CSMonster.BB_CSMonster'"));
	if (BBRef.Succeeded())
	{
		BlackboardAsset = BBRef.Object;
	}
}


void ACSMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BlackboardAsset, Blackboardcomp))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}