#include "CSRogueMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "CSRogueMonster.h"

ACSRogueMonsterAIController::ACSRogueMonsterAIController()
{
	Blackboardcomp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTRef(TEXT("'/Game/Team01/RogueMonster/AI/BT_RogueMonster.BT_RogueMonster'"));
	if (BTRef.Succeeded())
	{
		BehaviorTreeAsset = BTRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBRef(TEXT("'/Game/Team01/RogueMonster/AI/BB_RogueMonster.BB_RogueMonster'"));
	if (BBRef.Succeeded())
	{
		BlackboardAsset = BBRef.Object;
	}
}

void ACSRogueMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BlackboardAsset, Blackboardcomp))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}