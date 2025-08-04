#include "SpikeTrap.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	TriggerZone->SetupAttachment(Root);
	TriggerZone->SetCollisionProfileName(TEXT("Trigger"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(Root);

	SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
	SpikeMesh->SetupAttachment(BaseMesh);

}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnTriggerBegin);
	TriggerZone->OnComponentEndOverlap.AddDynamic(this, &ASpikeTrap::OnTriggerEnd);

	StartLocation = SpikeMesh->GetRelativeLocation();
	TargetLocation = StartLocation + SpikeOffset;
}

void ASpikeTrap::OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Trigger Begin Overlap"));

	if (!OtherActor)
	{
		UE_LOG(LogTemp, Error, TEXT("OtherActor is nullptr!"));
		return;
	}

	if (!OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor has no Player tag"));
		return;
	}

	if (!OtherActor->ActorHasTag("Player"))
	{

		return;
	}
	bMovingUp = true;
	// 타이머 등록 시도 로그
	UE_LOG(LogTemp, Warning, TEXT("Trying to register timer..."));

	GetWorld()->GetTimerManager().SetTimer(SpikeMoveTimer, this, &ASpikeTrap::MoveSpike, 0.01f, true);

	if (GetWorld()->GetTimerManager().IsTimerActive(SpikeMoveTimer))
	{
		UE_LOG(LogTemp, Warning, TEXT("active"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("inactive"));
	}
}

void ASpikeTrap::OnTriggerEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag("Player"))
	{
		return;
	}

	bMovingUp = false;
	GetWorld()->GetTimerManager().SetTimer(SpikeMoveTimer, this, &ASpikeTrap::MoveSpike, 0.01f, true);
}

void ASpikeTrap::MoveSpike()
{
	UE_LOG(LogTemp, Warning, TEXT("MoveSpike called"));
	FVector Current = SpikeMesh->GetRelativeLocation();
	FVector Target = bMovingUp ? TargetLocation : StartLocation;

	FVector NewLoc = FMath::VInterpConstantTo(Current, Target, GetWorld()->GetDeltaSeconds(), MoveSpeed);
	SpikeMesh->SetRelativeLocation(NewLoc);

	UE_LOG(LogTemp, Warning, TEXT("NewLoc: %s"), *NewLoc.ToString());

	if (FVector::Dist(NewLoc, Target) < 0.5f)
	{
		GetWorld()->GetTimerManager().ClearTimer(SpikeMoveTimer);
		if (bMovingUp)
		{
			ApplyDamage();
		}
	}
}

void ASpikeTrap::ApplyDamage()
{
	TArray<AActor*> HitActors;
	SpikeMesh->GetOverlappingActors(HitActors, ACharacter::StaticClass());

	for (AActor* Victim : HitActors)
	{
		UGameplayStatics::ApplyDamage(Victim, DamageAmount, GetInstigatorController(), this, nullptr);
	}
}
