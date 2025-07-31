#include "ChestItem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"



AChestItem::AChestItem()
{

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BodyMesh);

	if (IsValid(StaticMesh))
	{
		StaticMesh->SetHiddenInGame(true);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMesh->SetVisibility(false);
	}

	bIsOpened = false;
	CachedActivator = nullptr;
}

void AChestItem::BeginPlay()
{
	Super::BeginPlay();
}

void AChestItem::ActivateItem_Implementation(AActor* Activator)
{
	if (bIsOpened || !IsValid(Activator))
	{
		return;
	}
	
	CachedActivator = Activator;
	OpenChest();
	
}

void AChestItem::OpenChest()
{
	bIsOpened = true;
	UE_LOG(LogTemp, Warning, TEXT("box was opened"));

	TargetLidRotation = FRotator(-110.f, 0.f, 0.f);
	GetWorld()->GetTimerManager().SetTimer(LidOpenTimer, this, &AChestItem::TickLidRotation, 0.01f, true);

}

void AChestItem::TickLidRotation()
{
	const FRotator Current = LidMesh->GetRelativeRotation();
	const FRotator NewRotation = FMath::RInterpConstantTo(Current, TargetLidRotation, GetWorld()->GetDeltaSeconds(), 90.f);
	LidMesh->SetRelativeRotation(NewRotation);

	if (NewRotation.Equals(TargetLidRotation, 1.f))
	{
		GetWorld()->GetTimerManager().ClearTimer(LidOpenTimer);
	}
}