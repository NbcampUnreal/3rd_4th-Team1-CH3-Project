#include "ChestItem.h"



AChestItem::AChestItem()
{
	ItemType = "Chest";
	bIsOpened = false;
}

void AChestItem::ActivateItem(AActor* Activator)
{
	if (!bIsOpened)
	{
		bIsOpened = true;
		OpenChest();
	}
}

void AChestItem::OpenChest()
{
	UE_LOG(LogTemp, Warning, TEXT("box was opened"));
}