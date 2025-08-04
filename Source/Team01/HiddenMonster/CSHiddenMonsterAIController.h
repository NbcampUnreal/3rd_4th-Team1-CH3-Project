#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "CSHiddenMonsterAIController.generated.h"

class UBlackboardComponent;
class UBehaviorComponent;

UCLASS()
class TEAM01_API ACSHiddenMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACSHiddenMonsterAIController();

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
