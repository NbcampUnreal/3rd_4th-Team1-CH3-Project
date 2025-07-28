#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSBossMonster.generated.h"

UCLASS()
class TEAM01_API ACSBossMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// 생성자
	ACSBossMonster();

protected:
	// 게임 시작 이벤트
	virtual void BeginPlay() override;

public:	
	// 매 프레임 호출 이벤트
	virtual void Tick(float DeltaTime) override;

	// 입력 처리
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 보스 능력치를 관리할 컴포넌트
	class UCSBossStatComponent* StatComponent;
};
