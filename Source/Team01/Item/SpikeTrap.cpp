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
	//차이확인
	UE_LOG(LogTemp, Warning, TEXT("StartLocation: %s"), *StartLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("TargetLocation: %s"), *TargetLocation.ToString());
}

void ASpikeTrap::OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	
	bMovingUp = true;
	
	GetWorld()->GetTimerManager().SetTimer(SpikeMoveTimer, this, &ASpikeTrap::MoveSpike, 0.01f, true);

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

	if (FVector::Dist(NewLoc, Target) < 1.f)
	{
		SpikeMesh->SetRelativeLocation(Target);
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
	UE_LOG(LogTemp, Warning, TEXT("dd: %d"), HitActors.Num());

	for (AActor* Victim : HitActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("피격 대상: %s"), *Victim->GetName());
		UGameplayStatics::ApplyDamage(Victim, DamageAmount, GetInstigatorController(), this, nullptr);
	}
}
