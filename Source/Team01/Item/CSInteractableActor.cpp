#include "CSInteractableActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"

ACSInteractableActor::ACSInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;


	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	SetRootComponent(InteractionVolume);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACSInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ACSInteractableActor::OnOverlapBegin);
	InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ACSInteractableActor::OnOverlapEnd);

}

		//Interact 함수 구현
void ACSInteractableActor::Interact_Implementation(ACharacter* Interactor)
{

}

		// Overlap 시작 시 처리
void ACSInteractableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{

}

		// Overlap 종료 시 처리
void ACSInteractableActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}