#pragma once

#include "CoreMinimal.h"
#include "CSBaseStatComponent.h"
#include "CSBossStatComponent.generated.h"


UCLASS()
class TEAM01_API UCSBossStatComponent : public UCSBaseStatComponent
{
	GENERATED_BODY()

public:
	virtual void TakeDamage(float Damage) override; //부모 TakeDamage 함수 재정의 하기

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float SpecialGauge; //보스 전용 스텟 아직 실험용
};
