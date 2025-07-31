#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};



class TEAM01_API IItemInterface
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent)
    void OnItemOverlap(AActor* OverlapActor);

    UFUNCTION(BlueprintNativeEvent)
    void OnItemEndOverlap(AActor* OverlapActor);

    UFUNCTION(BlueprintNativeEvent)
    void ActivateItem(AActor* Activator);

    UFUNCTION(BlueprintNativeEvent)
    FName GetItemType() const;
};
