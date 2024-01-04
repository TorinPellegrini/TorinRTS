// Copyright Epic Games, Inc. All Rights Reserved.

#include "TorinRTSCharacter.h"

#include "CborTypes.h"
#include "AI/SAIController.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

ATorinRTSCharacter::ATorinRTSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovementComponent = GetCharacterMovement();
	if(CharacterMovementComponent)
	{
		CharacterMovementComponent->bOrientRotationToMovement = true; // Rotate character to moving direction
		CharacterMovementComponent->RotationRate = FRotator(0.f, 640.f, 0.f);
		CharacterMovementComponent->bConstrainToPlane = true;
		CharacterMovementComponent->bSnapToPlaneAtStart = true;
		MaxSpeed = CharacterMovementComponent->MaxWalkSpeed;
	}

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ATorinRTSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if(ShouldOrientate)
	{
		SetOrientation(DeltaSeconds);

		if(IsOriented())
		{
			ShouldOrientate = 0;
		}
	}
}

FTransform ATorinRTSCharacter::GetPositionTransform(const FVector Position) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FVector TraceOrigin = Position;
	TraceOrigin.Z += 10000.f;
	FVector TraceEnd = Position;
	TraceEnd.Z -= 10000.f;

	if(UWorld* WorldContext = GetWorld())
	{
		if(WorldContext->LineTraceSingleByChannel(Hit, TraceOrigin, TraceEnd, ECC_GameTraceChannel1, CollisionParams))
		{
			if(Hit.bBlockingHit)
			{
				FTransform HitTransform;

				HitTransform.SetLocation(Hit.ImpactPoint + FVector(1.f, 1.f, 1.25f));

				FRotator TerrainRotation = UKismetMathLibrary::MakeRotFromZX(Hit.Normal, FVector::UpVector);
				TerrainRotation += FRotator(90.f, 0.f, 0.f);
				HitTransform.SetRotation(TerrainRotation.Quaternion());
				
				return HitTransform;
			}
		}
	}

	return FTransform::Identity;
}

void ATorinRTSCharacter::Select()
{
	Selected = true;
	Highlight(Selected);
}

void ATorinRTSCharacter::DeSelect()
{
	Selected = false;
	Highlight(Selected);
}

void ATorinRTSCharacter::Highlight(const bool Highlight)
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for(UPrimitiveComponent* VisualComp : Components)
	{
		if(UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(Highlight);
		}
	}
}

void ATorinRTSCharacter::CommandMoveToLocation(const FCommandData CommandData)
{
	switch (CommandData.Type)
	{
	case ECommandType::CommandMoveSlow:
		{
			SetWalk();
			break;
		}
	case ECommandType::CommandMoveFast:
		{
			SetSprint();
			break;
		}
	case ECommandType::CommandMoveAttack:
		{
			
		}
	default:
		{
			SetRun();
		}
	}

	//Request AI Move
	CommandMove(CommandData);
}

void ATorinRTSCharacter::CommandMove(const FCommandData CommandData)
{
	//Clear delegate to remove current move in progress
	if(!SAIController)
	{
		return;
	}
	SAIController->OnReachedDestination.Clear();

	SAIController->OnReachedDestination.AddDynamic(this, &ATorinRTSCharacter::DestinationReached);
	
	SAIController->CommandMove(CommandData);
	SetMoveMarker(CommandData.Location);
}

void ATorinRTSCharacter::DestinationReached(const FCommandData CommandData)
{
	if(MoveMarker)
	{
		MoveMarker->Destroy();
	}
	TargetOrientation = CommandData.Rotation;
	ShouldOrientate = 1;
}

void ATorinRTSCharacter::SetWalk() const
{
	if(CharacterMovementComponent)
	{
		CharacterMovementComponent->MaxWalkSpeed = MaxSpeed * 0.5f;
	}
}

void ATorinRTSCharacter::SetRun() const
{
	if(CharacterMovementComponent)
	{
		CharacterMovementComponent->MaxWalkSpeed = MaxSpeed;
	}
}

void ATorinRTSCharacter::SetSprint() const
{
	if(CharacterMovementComponent)
	{
		CharacterMovementComponent->MaxWalkSpeed = MaxSpeed * 1.25f;
	}
}

void ATorinRTSCharacter::SetOrientation(const float DeltaTime)
{
	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, 0.f), TargetOrientation, DeltaTime, 2.f);
	SetActorRotation(InterpolatedRotation);
}

bool ATorinRTSCharacter::IsOriented() const
{
	const FRotator CurrentRotation = GetActorRotation();
	if(FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetOrientation.Yaw, 0.25f))
	{
		return true;
	}

	return false;
}

void ATorinRTSCharacter::SetMoveMarker(const FVector Location)
{
	if(MoveMarkerClass)
	{
		if(MoveMarker)
		{
			MoveMarker->Destroy();
		}

		FActorSpawnParameters Params;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if(UWorld* WorldContext = GetWorld())
		{
			MoveMarker = WorldContext->SpawnActor<AActor>(MoveMarkerClass, GetPositionTransform(Location), Params);
		}
	}
}
