#include "../Ui/CSGameClear.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"  
#include "GameFramework/PlayerController.h"

void UCS_WBP_GameClearUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (ClearMessageText)
	{
		ClearMessageText->SetText(FText::FromString(TEXT("축하합니다! 클리어!")));
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UCS_WBP_GameClearUI::OnRestartClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UCS_WBP_GameClearUI::OnExitClicked);
	}
}

void UCS_WBP_GameClearUI::SetFinalScore(int32 Score)
{
	if (ClearScoreText)
	{
		const FString ScoreString = FString::Printf(TEXT("최종 점수: %d"), Score);
		ClearScoreText->SetText(FText::FromString(ScoreString));
	}
}

void UCS_WBP_GameClearUI::OnRestartClicked()
{
	UGameplayStatics::OpenLevel(this, FName("MainLevel")); // 현재 맵 다시 로딩
}

void UCS_WBP_GameClearUI::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}