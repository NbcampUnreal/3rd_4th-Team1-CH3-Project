#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSGameClear.generated.h"


class UTextBlock;
class UButton;

UCLASS()
class TEAM01_API UCS_WBP_GameClearUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ClearMessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ClearScoreText; 

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnExitClicked();

public:
	void SetFinalScore(int32 Score);
};