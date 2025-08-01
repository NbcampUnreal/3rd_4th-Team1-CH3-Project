#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Team01/Character/CSCharacterBase.h"
#include "CSBossAnimInstance.generated.h"

UCLASS()
class TEAM01_API UCSBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UCSBossAnimInstance();

	//매 프레임 호출되어 애니메이션 로직을 업데이트하는 함수 (BP의 이벤트그래프 역할)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 이 애님 인스턴스를 소유한 폰(보스 몬스터)을 저장할 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<class ACSCharacterBase> OwningCharacter;

	// 캐릭터의 현재 속도를 저장할 변수입니다. 블루프린트에서 읽기만 가능하도록 설정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	float Speed;

	//현재 상태를 확인
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECharacterState CurrentState;
};
