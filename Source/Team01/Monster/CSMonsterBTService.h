#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CSMonsterBTService.generated.h"

UCLASS()
class TEAM01_API UCSMonsterBTService : public UBTService
{
	GENERATED_BODY()

public:
	UCSMonsterBTService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float DetectRange = 800.0f;
};
