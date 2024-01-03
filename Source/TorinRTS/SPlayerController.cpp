// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include <Windows.UI.Input.h>

#include "AIData.h"
#include "EnhancedInputSubsystems.h"
#include "PlacementPreview.h"
#include "Selectable.h"
#include "Net/UnrealNetwork.h"


ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer)
{
	bPlacementModeEnabled = false;
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

void ASPlayerController::Handle_DeSelection(AActor* ActorToSelect)
{
	if(ActorToSelect && ActorSelected(ActorToSelect))
	{
		Server_DeSelect(ActorToSelect);
	}
}

void ASPlayerController::Handle_DeSelection(TArray<AActor*> ActorsToSelect)
{
	Server_DeSelect_Group(ActorsToSelect);
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

FVector ASPlayerController::GetMousePositionOnSurface() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;
	if(GetWorld()->LineTraceSingleByChannel(OutHit,WorldLocation,WorldLocation+(WorldDirection*100000.f),ECollisionChannel::ECC_Visibility))
	{
		if(OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}
	return FVector::ZeroVector;
}

void ASPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bPlacementModeEnabled && PlacementPreviewActor)
	{
		PlacementUpdate();
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

void ASPlayerController::Server_DeSelect_Group_Implementation(const TArray<AActor*>& ActorsToDeSelect)
{
	for(int i = 0; i < ActorsToDeSelect.Num(); i++)
	{
		if(ActorsToDeSelect[i])
		{
			for(int j = Selected.Num() - 1; j >= 0; j--)
			{
				if(ActorsToDeSelect[i] == Selected[j])
				{
					if(ISelectable* Selectable = Cast<ISelectable>(ActorsToDeSelect[i]))
					{
						Selectable->DeSelect();
						Selected.RemoveAt(j);
						break;
					}
				}
			}
		}
	}

	OnRep_Selected();
}

void ASPlayerController::OnRep_Selected()
{
	OnSelectedUpdated.Broadcast();
}

/** Enhanced Input **/

void ASPlayerController::CommandSelected(FCommandData CommandData)
{
	Server_CommandSelected(CommandData);
}

void ASPlayerController::Server_CommandSelected_Implementation(FCommandData CommandData)
{
	if(!HasAuthority())
	{
		return;
	}

	for(int i = 0; i < Selected.Num(); i++)
	{
		if(ATorinRTSCharacter* SelectedCharacter = Cast<ATorinRTSCharacter>(Selected[i]))
		{
			SelectedCharacter->CommandMoveToLocation(CommandData);
		}
	}
}

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

void ASPlayerController::SetInputPlacement(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is Null!"));
	
	if(const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextPlacement);
		if(Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextPlacement, PlayerActions->MapPriorityPlacement);
			SetInputDefault(!Enabled);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextPlacement);
			SetInputDefault();
		}
	}
}

void ASPlayerController::SetInputShift(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is Null!"));

	if(const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextShift);
		if(Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextShift, PlayerActions->MapPriorityShift);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextShift);
		}
	}
}

void ASPlayerController::SetInputAlt(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is Null!"));

	if(const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextAlt);
		if(Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextAlt, PlayerActions->MapPriorityAlt);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextAlt);
		}
	}
}

void ASPlayerController::SetInputCtrl(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is Null!"));

	if(const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextCtrl);
		if(Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextCtrl, PlayerActions->MapPriorityCtrl);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextCtrl);
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

void ASPlayerController::SetPlacementPreview()
{
	if(PreviewActorType && !bPlacementModeEnabled)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetMousePositionOnSurface());
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PlacementPreviewActor = GetWorld()->SpawnActor<APlacementPreview>(PreviewActorType, SpawnTransform, SpawnParameters);

		if(PlacementPreviewActor)
		{
			//Placement input enable
			SetInputPlacement();

			//Placement mode
			bPlacementModeEnabled = true;
		}
	}
}

void ASPlayerController::Place()
{
	if(!IsPlacementModeEnabled() || !PlacementPreviewActor)
	{
		return;
	}

	//Disable placing
	bPlacementModeEnabled = false;

	//Disable placement controls
	SetInputPlacement(false);

	//Send to server (if multiplayer is implemented)
	Server_Place(PlacementPreviewActor);
}

void ASPlayerController::PlaceCancel()
{
	if(!IsPlacementModeEnabled() || !PlacementPreviewActor)
	{
		return;
	}

	//Disable placement mode
	bPlacementModeEnabled = false;

	//Disable placement input
	SetInputPlacement(false);

	EndPlacement();
}

void ASPlayerController::PlacementUpdate() const
{
	if(!PlacementPreviewActor)
	{
		return;
	}

	PlacementPreviewActor->SetActorLocation(GetMousePositionOnSurface());
}

void ASPlayerController::Server_Place_Implementation(AActor* PlacementPreviewToSpawn)
{
	if(const APlacementPreview* Preview = Cast<APlacementPreview>(PlacementPreviewToSpawn))
	{
		FTransform SpawnTransform;
		FVector Location = Preview->GetActorLocation();
		SpawnTransform.SetLocation(FVector(Location.X, Location.Y, Location.Z + 100.f));
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if(ATorinRTSCharacter* NewUnit = GetWorld()->SpawnActor<ATorinRTSCharacter>(Preview->PlaceableClass, SpawnTransform, SpawnParameters))
		{
			NewUnit->SetOwner(this);
		}
	}

	EndPlacement();
}

void ASPlayerController::EndPlacement_Implementation()
{
	PlacementPreviewActor->Destroy();
}
