#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CSRogueMonsterAIController.generated.h"

UCLASS()
class TEAM01_API ACSRogueMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ACSRogueMonsterAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	UBlackboardComponent* Blackboardcomp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBlackboardData* BlackboardAsset;
};
