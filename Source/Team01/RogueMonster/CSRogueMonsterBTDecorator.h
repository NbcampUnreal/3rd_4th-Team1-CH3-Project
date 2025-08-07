#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CSRogueMonsterBTDecorator.generated.h"

UENUM(BlueprintType)
enum class ERogueAIConditionType : uint8
{
	IsDead,
	IsAttacking,
	IsRangeAttacking,
	IsHit,
	IsDetectedPlayer
};

UCLASS()
class TEAM01_API UCSRogueMonsterBTDecorator : public UBTDecorator
{
	GENERATED_BODY()

public:
	UCSRogueMonsterBTDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const override;

public:
	UPROPERTY(EditAnywhere, Category = "Condition")
	ERogueAIConditionType ConditionType;

};