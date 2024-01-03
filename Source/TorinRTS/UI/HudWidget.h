// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FormationSelectorWidget.h"
#include "Blueprint/UserWidget.h"
#include "TorinRTS/SPlayerController.h"
#include "HudWidget.generated.h"

class UFormationSelectorWidget;

/**
 * 
 */
UCLASS()
class TORINRTS_API UHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationSelectorWidget* FormationSelectorWidget;

protected:
	UFUNCTION()
	void SetFormationSelectionWidget(const bool bEnabled) const;

	UFUNCTION()
	void OnSelectionUpdated();

	UPROPERTY()
	ASPlayerController* SPlayer;
	
};
