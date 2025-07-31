#include "PotionItem.h"
#include "CSPlayerCharacter.h"
#include "GameFramework/Actor.h"

APotionItem::APotionItem()
{
    ItemType = "Potion";
}

void APotionItem::OnItemOverlap(AActor* OverlapActor)
{
    if (ACSPlayerCharacter* Player = Cast<ACSPlayerCharacter>(OverlapActor))
    {
        Player->RecoverHealth(50.f);
        DestroyItem();
    }
}
