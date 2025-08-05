#include "PortalActor.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


APortalActor::APortalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(Root);

	PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(Root);

	TargetLocation = FVector::ZeroVector;
}

void APortalActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APortalActor::ActivateItem_Implementation(AActor* Activator)
{
	if (!IsValid(Activator))
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("MOVE!!"));
	Activator->SetActorLocation(TargetLocation);
}
