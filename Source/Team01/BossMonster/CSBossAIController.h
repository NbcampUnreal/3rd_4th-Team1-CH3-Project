#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CSBossAIController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM01_API ACSBossAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ACSBossAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override; // 보스몬스터에 빙의 되었을 때 호출

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BossBehaviorTree; // AI 컨트롤러가 어떤 비헤이비어 트리 에셋을 실행해야하는가?
};
