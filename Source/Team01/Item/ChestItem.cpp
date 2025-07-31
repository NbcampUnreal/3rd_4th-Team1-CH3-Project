#include "ChestItem.h"



AChestItem::AChestItem()
{
	bIsOpened = false;
	CachedActivator = nullptr;
}

void AChestItem::BeginPlay()
{
	Super::BeginPlay();
}

void AChestItem::ActivateItem(AActor* Activator)
{
	if (!bIsOpened || Activator == nullptr)
	{
		return;
	}
	
	CachedActivator = Activator;
	OpenChest();
	
}

void AChestItem::OpenChest()
{
	UE_LOG(LogTemp, Warning, TEXT("box was opened"));
}