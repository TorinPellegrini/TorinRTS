// Fill out your copyright notice in the Description page of Project Settings.


#include "InputTriggerDoubleTap.h"

#include "EnhancedPlayerInput.h"

ETriggerState UInputTriggerDoubleTap::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                 FInputActionValue ModifiedValue, float DeltaTime)
{
	if(IsActuated(ModifiedValue) && !IsActuated(LastValue))
	{
		const float CurrentTime = PlayerInput->GetOuterAPlayerController()->GetWorld()->GetRealTimeSeconds();
		if(CurrentTime - LastTapTime < Delay)
		{
			LastTapTime = 0;
			return ETriggerState::Triggered;
		}
		LastTapTime = CurrentTime;
	}
	return  ETriggerState::None;
}

