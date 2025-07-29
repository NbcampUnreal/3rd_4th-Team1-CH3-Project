#include "CSMonsterBTDecorator.h"
#include "AIController.h"
#include "Team01/Character/CSCharacterBase.h"

UCSMonsterBTDecorator::UCSMonsterBTDecorator()
{
	NodeName = TEXT("Check Character Cindition");
}

bool UCSMonsterBTDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
}