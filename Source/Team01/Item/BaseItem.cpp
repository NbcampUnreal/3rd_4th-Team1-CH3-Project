#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ABaseItem::ABaseItem()
{
    PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    RootComponent = Scene;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->SetupAttachment(Scene);
    Collision->SetSphereRadius(100.f);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionObjectType(ECC_WorldDynamic);
    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::HandleBeginOverlap);
    Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::HandleEndOverlap);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(Scene);
}

void ABaseItem::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    OnItemOverlap(OtherActor);
}

void ABaseItem::HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    OnItemEndOverlap(OtherActor);
}

void ABaseItem::OnItemOverlap(AActor* OverlapActor)
{
    // �ڽĿ��� ����
}

void ABaseItem::OnItemEndOverlap(AActor* OverlapActor)
{
    // �ڽĿ��� ����
}

void ABaseItem::ActivateItem(AActor* Activator)
{
    // �ڽĿ��� ����
}

FName ABaseItem::GetItemType() const
{
    return ItemType;
}

void ABaseItem::DestroyItem()
{
    Destroy();
}
