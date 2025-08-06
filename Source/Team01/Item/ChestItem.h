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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* OpenSound;

	FTimerHandle LidOpenTimer;
	FRotator TargetLidRotation;

	UPROPERTY(EditAnywhere, Category = "Chest")
	TSubclassOf<ABaseItem> PotionItemClass;

	UPROPERTY(EditAnywhere, Category = "Chest")
	TSubclassOf<AActor> MonsterClass;

	UPROPERTY(EditAnywhere, Category = "Chest|Stats")
	float MaxHP = 10.f;

	float CurrentHP;

	bool bIsOpened;

	UPROPERTY()
	AActor* CachedActivator;


	void OpenChest();
	void TickLidRotation();
	void DropPotion();

public:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
};
