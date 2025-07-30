#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_WBP_KillConfirm.generated.h"

class UTextBlock;

UCLASS()
class TEAM01_API UCS_WBP_KillConfirm : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// 텍스트 설정 함수
	void ShowKillMessage(const FString& Message);
	void HideMessage();
	
protected:
	FTimerHandle HideTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> KillConfirm; // "Kill!!" 텍스트
};
