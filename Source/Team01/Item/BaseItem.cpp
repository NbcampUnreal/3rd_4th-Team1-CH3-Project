#include "BaseItem.h"
#include "Components/SphereComponent.h"

ABaseItem::ABaseItem()
{
    PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->SetupAttachment(Scene);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(Collision);
}

void ABaseItem::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(Collision))
    {
        Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnOverlapBegin);
    }
}

void ABaseItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        IItemInterface::Execute_OnItemOverlap(this, OtherActor);
        UE_LOG(LogTemp, Warning, TEXT("BaseItem Overlap: %s"), *OtherActor->GetName());
    }
}

void ABaseItem::OnItemOverlap_Implementation(AActor* OverlapActor) 
{
}
void ABaseItem::OnItemEndOverlap_Implementation(AActor* OverlapActor) 
{
}
void ABaseItem::ActivateItem_Implementation(AActor* Activator) 
{
}
FName ABaseItem::GetItemType_Implementation() const 
{
    return ItemType;
}

void ABaseItem::DestroyItem()
{
    Destroy();
}
