#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ChestItem.generated.h"


UCLASS()
class TEAM01_API AChestItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AChestItem();

protected:

	virtual void BeginPlay() override;
	virtual void

	UPROPERTY()
	bool bIsOpened;

	UPROPERTY()
	AActor* CachedActivator;


	virtual void ActivateItem(AActor* Activator) override;
	void OpenChest();
	
};
