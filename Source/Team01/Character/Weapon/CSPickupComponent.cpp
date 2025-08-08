// Fill out your copyright notice in the Description page of Project Settings.


#include "CSPickupComponent.h"
#include "../CSPlayerCharacter.h"

UCSPickupComponent::UCSPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCSPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(
		this, &ThisClass::HandleOnComponentBeginOverlap);
}

void UCSPickupComponent::HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACSPlayerCharacter* OverlappedCharacter = Cast<ACSPlayerCharacter>(OtherActor);
	if (IsValid(OverlappedCharacter))
	{
		OnPickup.Broadcast(OverlappedCharacter);

		OnComponentBeginOverlap.RemoveAll(this);

		if (OverlappedCharacter->AddGrenade(1))
		{
			GetOwner()->Destroy();
		}
	}
}
