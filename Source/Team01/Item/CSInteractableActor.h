#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSInteractableActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ACharacter;

UCLASS()
class TEAM01_API ACSInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	ACSInteractableActor();

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(ACharacter* Interactor);

	virtual void Interact_Implementation(ACharacter* Interactor);

protected:

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);




};
