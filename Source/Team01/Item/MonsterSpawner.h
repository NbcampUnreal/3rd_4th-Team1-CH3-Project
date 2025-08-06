#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class TEAM01_API AMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:
    AMonsterSpawner();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<AActor> MonsterClass;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    int32 MaxSpawnCount = 3;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnRadius = 300.f;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnInterval = 5.0f;

    int32 CurrentSpawnCount = 0;

    FTimerHandle AutoSpawnTimer;

    void StartAutoSpawn();

public:
    UFUNCTION(BlueprintCallable)
    void SpawnMonster();
};