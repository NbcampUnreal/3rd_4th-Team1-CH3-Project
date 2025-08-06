#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CSMummyAIController.generated.h"

UCLASS()
class TEAM01_API ACSMummyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACSMummyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	UBlackboardComponent* Blackboardcomp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBlackboardData* BlackboardAsset;

	void InitPatrolTarget(APawn* InPawn);
	
};
