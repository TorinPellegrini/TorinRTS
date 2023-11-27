// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Windows.UI.Input.h>

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"
#include "PlayerInputActions.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS(BlueprintType)
class TORINRTS_API UPlayerInputActions : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Default Player Mapping Context */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInputMappingContext* MappingContextDefault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 MapPriorityDefault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInputAction* Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInputAction* Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInputAction* Zoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInputAction* Select;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInputAction* Rotate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInputAction* TestPlacement;

	/** Placement Player Mapping Context */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Placement")
	UInputMappingContext* MappingContextPlacement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Placement")
	int32 MapPriorityPlacement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Placement")
	UInputAction* Place;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Placement")
	UInputAction* PlaceCancel;
	
};

namespace EPlayerInputActions
{
	template<class T, class FuncType>
	void BindInput_TriggerOnly(UEnhancedInputComponent* Input, const UInputAction* Action, T* Obj, FuncType TriggerFunc)
	{
		if(TriggerFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Triggered, Obj, TriggerFunc);
		}
	}

	template<class T, class FuncType>
	void BindInput_StartTriggerComplete(UEnhancedInputComponent* Input, const UInputAction* Action, T* Obj, FuncType StartFunc, FuncType TriggerFunc, FuncType CompleteFunc)
	{
		if(StartFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Started, Obj, StartFunc);
		}

		if(TriggerFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Triggered, Obj, TriggerFunc);
		}

		if(CompleteFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Completed, Obj, CompleteFunc);
		}
	}
}
