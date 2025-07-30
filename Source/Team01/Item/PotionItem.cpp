#include "PotionItem.h"
//#include "Character/CSPlayerCharacter.h"

APotionItem::APotionItem()
{
    ItemType = "Potion";
}

void APotionItem::OnItemOverlap(AActor* OverlapActor)
{
    //if (CSPlayerCharacter* Player = Cast<CSPlayerCharacter>(OverlapActor))
    //{
    //    Player->RecoverHealth(50.f);
    //    DestroyItem();
    //}
}
