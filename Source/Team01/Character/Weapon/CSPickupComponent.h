// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "CSPickupComponent.generated.h"

class ACSPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickup, ACSPlayerCharacter*, InPickupCharacter);
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta = (BlueprintSpawnableComponent))
class TEAM01_API UCSPickupComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UCSPickupComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(BlueprintAssignable)
	FOnPickup OnPickup;
	
};
