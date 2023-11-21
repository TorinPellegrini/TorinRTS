// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include <Windows.UI.Input.h>

#include "EnhancedInputSubsystems.h"
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

void ASPlayerController::Handle_Selection(TArray<AActor*> ActorsToSelect)
{
	Server_Select_Group(ActorsToSelect);
}

FVector ASPlayerController::GetMousePositionOnTerrain() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;
	if(GetWorld()->LineTraceSingleByChannel(OutHit,WorldLocation,WorldLocation+(WorldDirection*100000.f),ECollisionChannel::ECC_GameTraceChannel1))
	{
		if(OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}
	return FVector::ZeroVector;
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

void ASPlayerController::Server_Select_Group_Implementation(const TArray<AActor*>& ActorsToSelect)
{
	Server_ClearSelected();
	
	TArray<AActor*> ValidActors;
	for(int i=0; i < ActorsToSelect.Num(); ++i)
	{
		if(ActorsToSelect[i])
		{
			if(ISelectable* Selectable = Cast<ISelectable>(ActorsToSelect[i]))
			{
				ValidActors.Add(ActorsToSelect[i]);
				Selectable->Select();
			}
		}
	}


	Selected.Append(ValidActors);
	OnRep_Selected();
	ValidActors.Empty();
	
}

void ASPlayerController::Server_DeSelect_Implementation(AActor* ActorToDeSelect)
{
	if(ActorToDeSelect)
	{
		if(ISelectable* Selectable = Cast<ISelectable>(ActorToDeSelect))
		{
			Selectable->DeSelect();
			Selected.Remove(ActorToDeSelect);
			OnRep_Selected();
		}
	}
	
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
			OnRep_Selected();
		}
	}
}

void ASPlayerController::Server_ClearSelected_Implementation()
{
	for(int i=0;i<Selected.Num(); ++i)
	{
		if(Selected[i])
		{
			if(ISelectable* Selectable = Cast<ISelectable>(Selected[i]))
			{
				Selectable->DeSelect();
			}
		}
	}

	Selected.Empty();
}

void ASPlayerController::OnRep_Selected()
{
	OnSelectedUpdated.Broadcast();
}

/** Enhanced Input **/

void ASPlayerController::AddInputMapping(const UInputMappingContext* InputMapping, const int32 MappingPriority) const
{
	if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(InputMapping);

		if(!InputSubsystem->HasMappingContext(InputMapping))
		{
			InputSubsystem->AddMappingContext(InputMapping, MappingPriority);
		}
	}
}

void ASPlayerController::RemoveInputMapping(const UInputMappingContext* InputMapping) const
{
	if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(InputMapping);
		InputSubsystem->RemoveMappingContext(InputMapping);
	}
}

void ASPlayerController::SetInputDefault(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is NULL!"));

	if(const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextDefault);
		if(Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextDefault, PlayerActions->MapPriorityDefault);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextDefault);
		}
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSubsystem->ClearAllMappings();
		SetInputDefault();
	}
}
