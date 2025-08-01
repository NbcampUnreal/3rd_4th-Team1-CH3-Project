#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "CS_WBP_EnemyHPBar.generated.h"

UCLASS()
class TEAM01_API UCS_WBP_EnemyHPBar : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHPBarPercent(float Percent);

	UFUNCTION(BlueprintCallable)
	void UpdateHP(float NewPercent);

	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	
};
