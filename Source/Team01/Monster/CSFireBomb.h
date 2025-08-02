#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSFireBomb.generated.h"

class UStaticMeshComponent;

UCLASS()
class TEAM01_API ACSFireBomb : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSFireBomb();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *StaticMesh;

	UPROPERTY(VisibleAnywhere)
	

};
