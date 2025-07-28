#include "CSBossMonster.h"
#include "CSBossStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACSBossMonster::ACSBossMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	StatComponent = CreateDefaultSubobject<UCSBossStatComponent>(TEXT("StatComponent")); // 스텟 컴포넌트 부착
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 이동하는 방향으로 자동으로 회전하도록 설정
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); //회전속도 설정

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // 캐릭터가 월드에 나타갔을 때 AI 컨트롤러가 자동으로 Possess 하도록 설정

}

void ACSBossMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACSBossMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACSBossMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

