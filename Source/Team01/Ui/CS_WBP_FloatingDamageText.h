#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_WBP_FloatingDamageText.generated.h"

class UTextBlock;

UCLASS()
class TEAM01_API UCS_WBP_FloatingDamageText : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// 데미지를 설정하는 함수
	void SetDamage(int32 DamageValue);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Damage; // 데미지 텍스트
};
