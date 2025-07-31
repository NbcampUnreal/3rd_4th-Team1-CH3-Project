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


    virtual void OnItemOverlap_Implementation(AActor* OverlapActor) override;


};
