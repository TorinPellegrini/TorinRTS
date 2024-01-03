// Fill out your copyright notice in the Description page of Project Settings.


#include "SAIController.h"

#include "TorinRTS/TorinRTSCharacter.h"

ASAIController::ASAIController(const FObjectInitializer& ObjectInitializer)
{
}

void ASAIController::CommandMove(FCommandData CommandData)
{
	CurrentCommandData = CommandData;
	MoveToLocation(CommandData.Location);
}

void ASAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


    //Set reference to the owner and set reference of the controller on the owner
	OwningCharacter = Cast<ATorinRTSCharacter>(InPawn);
	if(OwningCharacter)
	{
		OwningCharacter->SetAIController(this);
	}
}

void ASAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	OnReachedDestination.Broadcast(CurrentCommandData);
}
