#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInterface.h"
#include "BaseItem.generated.h"

class USphereComponent;

UCLASS()
class TEAM01_API ABaseItem : public AActor, public IItemInterface
{
    GENERATED_BODY()

public:
    ABaseItem();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemType;

    UPROPERTY(VisibleAnywhere)
    USceneComponent* Scene;

    UPROPERTY(VisibleAnywhere)
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* StaticMesh;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    virtual void BeginPlay() override;

public:
    virtual void OnItemOverlap_Implementation(AActor* OverlapActor) override;
    virtual void OnItemEndOverlap_Implementation(AActor* OverlapActor) override;
    virtual void ActivateItem_Implementation(AActor* Activator) override;
    virtual FName GetItemType_Implementation() const override;

protected:
    void DestroyItem();
};
