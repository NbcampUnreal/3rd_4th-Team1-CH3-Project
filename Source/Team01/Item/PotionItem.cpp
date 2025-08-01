#include "PotionItem.h"
#include "CSPlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

APotionItem::APotionItem()
{
    ItemType = "Potion";
}

void APotionItem::OnItemOverlap_Implementation(AActor* OverlapActor)
{
    if (ACSPlayerCharacter* Player = Cast<ACSPlayerCharacter>(OverlapActor))
    {
        if (PickupSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
        }          
     Player->RecoverHealth(50.f);
     DestroyItem();
        
    }
}

