// Fill out your copyright notice in the Description page of Project Settings.


#include "HudWidget.h"

#include "FormationSelectorWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TorinRTS/SPlayerController.h"

void UHudWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((SPlayer = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	//Disable formation selection widget
	SetFormationSelectionWidget(false);

	//Bind to player controller's selection update delegate
	if(SPlayer)
	{
		SPlayer->OnSelectedUpdated.AddDynamic(this, &UHudWidget::OnSelectionUpdated);
	}
}


void UHudWidget::SetFormationSelectionWidget(const bool bEnabled) const
{
	if(FormationSelectionWidget)
	{
		FormationSelectionWidget->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UHudWidget::OnSelectionUpdated()
{
	if(SPlayer)
	{
		SetFormationSelectionWidget(SPlayer->HasGroupSelection());
	}
}
