#include "MonsterSpawner.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();

    StartAutoSpawn();
}

void AMonsterSpawner::StartAutoSpawn()
{
    if (SpawnInterval > 0.f && MonsterClass)
    {
        GetWorld()->GetTimerManager().SetTimer(AutoSpawnTimer, this, &AMonsterSpawner::SpawnMonster, SpawnInterval, true);
    }
}

void AMonsterSpawner::SpawnMonster()
{
    if (!MonsterClass || CurrentSpawnCount >= MaxSpawnCount)
        return;

    FVector Origin = GetActorLocation();
    FVector2D Rand2D = FMath::RandPointInCircle(SpawnRadius);
    FVector SpawnLocation = Origin + FVector(Rand2D.X, Rand2D.Y, 50.f);

    AActor* Spawned = GetWorld()->SpawnActor<AActor>(MonsterClass, SpawnLocation, FRotator::ZeroRotator);
    if (Spawned)
    {
        CurrentSpawnCount++;
    }
}