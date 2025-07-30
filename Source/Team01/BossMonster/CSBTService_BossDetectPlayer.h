#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "CSBTService_BossDetectPlayer.generated.h"

UCLASS()
class TEAM01_API UCSBTService_BossDetectPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UCSBTService_BossDetectPlayer();

protected:
	// 주기적으로 Detecting 할 수 있도록 하는 함수
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// 보스의 공격 사정거리를 에디터에서 쉽게 바꿀 수 있도록 변수로 만듭니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float AttackRange;
};
