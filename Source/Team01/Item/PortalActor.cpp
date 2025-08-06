#include "PortalActor.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"


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

	InteractionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionZone"));
	InteractionZone->SetupAttachment(RootComponent);
	InteractionZone->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	InteractionZone->SetCollisionProfileName(TEXT("Trigger"));
	InteractionZone->SetGenerateOverlapEvents(true);

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

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}


	PC->PlayerCameraManager->StartCameraFade(0.f, 1.f, 0.25f, FLinearColor::Black, true, true);

	if (PortalWarpSound)
	{
		UGameplayStatics::SpawnSound2D(this, PortalWarpSound); 
	}

	FTimerHandle FadeTimer;

	GetWorld()->GetTimerManager().SetTimer(FadeTimer, [this, Activator, PC]()
		{
			Activator->SetActorLocation(TargetLocation);
			PC->PlayerCameraManager->StartCameraFade(1.f, 0.f, 1.0f, FLinearColor::Black, false, false);
		}, 1.0f, false);
}
