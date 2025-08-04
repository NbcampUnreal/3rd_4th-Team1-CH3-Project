#include "CS_WBP_KillConfirm.h"
#include "Components/TextBlock.h"

// 위젯 생성 시 호출됩니다
void UCS_WBP_KillConfirm::NativeConstruct()
{
	Super::NativeConstruct();

	// 기본 텍스트 설정 ("Kill!!")
	if (KillConfirm)
	{
		KillConfirm->SetText(FText::FromString(TEXT("Kill!!")));
	}
}

// 킬 메시지를 갱신합니다 ("헤드샷!" 등 텍스트로 설정 가능)
void UCS_WBP_KillConfirm::ShowKillMessage(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("[KillConfirm] ShowKillMessage called"));

	if (KillConfirm)
	{
		KillConfirm->SetText(FText::FromString(Message));
		SetVisibility(ESlateVisibility::Visible);

		// 타이머로 숨김 예약
		GetWorld()->GetTimerManager().ClearTimer(HideTimer);
		GetWorld()->GetTimerManager().SetTimer(HideTimer, this, &UCS_WBP_KillConfirm::HideMessage, 2.0f, false);
	}
	
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[KillConfirm] KillConfirm text is nullptr!"));
	}
}

void UCS_WBP_KillConfirm::HideMessage()
{
	SetVisibility(ESlateVisibility::Hidden);
	UE_LOG(LogTemp, Warning, TEXT("[KillConfirm] Hide"));
}