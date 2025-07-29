#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "CSMonsterAIController.generated.h"

UCLASS()
class TEAM01_API ACSMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACSMonsterAIController();

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
