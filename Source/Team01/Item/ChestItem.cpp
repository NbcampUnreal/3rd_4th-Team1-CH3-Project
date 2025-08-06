#include "ChestItem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"



AChestItem::AChestItem()
{

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	BodyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BodyMesh->SetCollisionObjectType(ECC_WorldDynamic);
	BodyMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BodyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	BodyMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	BodyMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BodyMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BodyMesh);

	if (IsValid(StaticMesh))
	{
		StaticMesh->SetHiddenInGame(true);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMesh->SetVisibility(false);
	}
	CurrentHP = MaxHP;
	bIsOpened = false;
	CachedActivator = nullptr;

	SetCanBeDamaged(true);
}

void AChestItem::BeginPlay()
{
	Super::BeginPlay();
}

void AChestItem::ActivateItem_Implementation(AActor* Activator)
{
	if (bIsOpened || !IsValid(Activator))
	{
		return;
	}
	
	CachedActivator = Activator;
	OpenChest();
	
	if (MonsterClass)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(100.f, 0.f, 50.f);
		GetWorld()->SpawnActor<AActor>(MonsterClass, SpawnLocation, FRotator::ZeroRotator);
	}
}

void AChestItem::OpenChest()
{
	bIsOpened = true;
	UE_LOG(LogTemp, Warning, TEXT("box was opened"));

	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
	}

	TargetLidRotation = FRotator(0.f, 0.f, -110.f);
	GetWorld()->GetTimerManager().SetTimer(LidOpenTimer, this, &AChestItem::TickLidRotation, 0.01f, true);

}

void AChestItem::TickLidRotation()
{
	const FRotator Current = LidMesh->GetRelativeRotation();
	const FRotator NewRotation = FMath::RInterpConstantTo(Current, TargetLidRotation, GetWorld()->GetDeltaSeconds(), 90.f);
	LidMesh->SetRelativeRotation(NewRotation);

	if (NewRotation.Equals(TargetLidRotation, 1.f))
	{
		GetWorld()->GetTimerManager().ClearTimer(LidOpenTimer);
	}
}

float AChestItem::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Chest TakeDamage : %.1f"), DamageAmount);

	if (bIsOpened)
		return 0.f;

	CurrentHP -= DamageAmount;

	if (CurrentHP <= 0.f)
	{
		DropPotion();
		Destroy();
	}

	return DamageAmount;
}

void AChestItem::DropPotion()
{
	if (!PotionItemClass) return;

	FVector DropLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
	GetWorld()->SpawnActor<ABaseItem>(PotionItemClass, DropLocation, FRotator::ZeroRotator);
}