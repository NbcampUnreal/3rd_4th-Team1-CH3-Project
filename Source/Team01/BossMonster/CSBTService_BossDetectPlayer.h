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
	// 보스의 Melee 공격 사정거리를 설정하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float AttackRange;

	// 보스의 Ground Slam 점프 공격 사정거리를 설정하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float JumpAttackRange;

	// 플레이어를 인식할 수 있는 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float DetectRange;
};
