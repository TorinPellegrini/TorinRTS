// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FormationSelectorWidget.generated.h"

class USlider;
class UFormationButtonWidget;
class USimpleUIButtonWidget;
class ASPlayerController;
/**
 * 
 */
UCLASS()
class TORINRTS_API UFormationSelectorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* LineButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* ColumnButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* WedgeButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* BlobButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* SpacingSlider;

protected:
	UFUNCTION()
	void OnFormationButtonClicked(USimpleUIButtonWidget* Button, int Index);

	UFUNCTION()
	void OnSpacingValueChanged(const float Value);

	UPROPERTY()
	ASPlayerController* SPlayer;
	
};
