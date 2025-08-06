#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "PotionItem.generated.h"

UCLASS()
class TEAM01_API APotionItem : public ABaseItem
{
    GENERATED_BODY()

public:
    APotionItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundBase* PickupSound;


    virtual void OnItemOverlap_Implementation(AActor* OverlapActor) override;


};
