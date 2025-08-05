#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInterface.h"
#include "PortalActor.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class TEAM01_API APortalActor : public AActor, public IItemInterface
{
	GENERATED_BODY()
	
public:	
	APortalActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Portal")
	UStaticMeshComponent* PortalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Portal")
	UNiagaraComponent* PortalEffect;

	UPROPERTY(EditAnywhere, Category = "Portal")
	FVector TargetLocation;

public:	

	virtual void ActivateItem_Implementation(AActor* Activator) override;

};
