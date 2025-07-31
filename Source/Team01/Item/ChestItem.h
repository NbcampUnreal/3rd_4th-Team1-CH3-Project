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
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LidMesh;

	FTimerHandle LidOpenTimer;
	FRotator TargetLidRotation;

private:

	UPROPERTY()
	bool bIsOpened;

	UPROPERTY()
	AActor* CachedActivator;


	void OpenChest();
	void TickLidRotation();
	
};
