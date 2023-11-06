// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "Selectable.h"
#include "Net/UnrealNetwork.h"


ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer)
{
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASPlayerController, Selected, COND_OwnerOnly);
}

void ASPlayerController::Handle_Selection(AActor* ActorToSelect)
{
	if(ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		if(ActorToSelect && ActorSelected(ActorToSelect))
		{
			Server_DeSelect(ActorToSelect);
		}
		else
		{
			Server_Select(ActorToSelect);
		}
	}
	else
	{
		Server_ClearSelected();
	}
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

bool ASPlayerController::ActorSelected(AActor* ActorToCheck) const
{
	if(ActorToCheck && Selected.Contains(ActorToCheck))
	{
		return true;
	}

	return false;
}

void ASPlayerController::Server_DeSelect_Implementation(AActor* ActorToSelect)
{
	
}

void ASPlayerController::Server_Select_Implementation(AActor* ActorToSelect)
{
	Server_ClearSelected();

	if(ActorToSelect)
	{
		if(ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
		{
			Selectable->Select();
			Selected.Add(ActorToSelect);
		}
	}
}

void ASPlayerController::Server_ClearSelected_Implementation()
{
}

void ASPlayerController::OnRep_Selected()
{
	OnSelectedUpdated.Broadcast();
}
