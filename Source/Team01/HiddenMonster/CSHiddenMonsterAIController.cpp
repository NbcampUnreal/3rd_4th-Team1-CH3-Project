#include "CSHiddenMonsterAIController.h"
#include "CSHiddenMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

ACSHiddenMonsterAIController::ACSHiddenMonsterAIController()
{
	Blackboardcomp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTRef(TEXT("'/Game/Team01/HiddenMonster/AI/BT_CSHiddenMonster.BT_CSHiddenMonster'"));
	if (BTRef.Succeeded())
	{
		BehaviorTreeAsset = BTRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBRef(TEXT("'/Game/Team01/HiddenMonster/AI/BB_CSHiddenMonster.BB_CSHiddenMonster'"));
	if (BBRef.Succeeded())
	{
		BlackboardAsset = BBRef.Object;
	}
}

void ACSHiddenMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BlackboardAsset, Blackboardcomp))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}
