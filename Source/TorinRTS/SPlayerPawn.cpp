// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerPawn.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASPlayerPawn::ASPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent=CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent=SceneComponent;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 2000.f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

}

// Called when the game starts or when spawned
void ASPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	//Set the initial desired zoom and location
	TargetLocation = GetActorLocation();
	TargetZoom = 3000.f;

	//Set initial rotation for the camera
	const FRotator Rotation = SpringArmComponent->GetRelativeRotation();
	TargetRotation = FRotator(Rotation.Pitch + -50.f, Rotation.Yaw, 0.0f);
	
}

void ASPlayerPawn::GetTerrainPosition(FVector& TerrainPosition) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FVector TraceOrigin = TerrainPosition;
	TraceOrigin.Z += 10000.f;
	FVector TraceEnd = TerrainPosition;
	TraceEnd.Z -= 10000.f;

	if(UWorld* WorldContext = GetWorld())
	{
		if(WorldContext->LineTraceSingleByChannel(Hit, TraceOrigin, TraceEnd, ECC_Visibility, CollisionParams))
		{
			TerrainPosition = Hit.ImpactPoint;
		}
	}
}

void ASPlayerPawn::Forward(float AxisValue)
{
	if(AxisValue==0.f)
	{
		return;
	}

	TargetLocation = SpringArmComponent->GetForwardVector() * AxisValue * MoveSpeed + TargetLocation;
	GetTerrainPosition(TargetLocation);
}

void ASPlayerPawn::Right(float AxisValue)
{
	if(AxisValue==0.f)
	{
		return;
	}

	TargetLocation = SpringArmComponent->GetRightVector() * AxisValue * MoveSpeed + TargetLocation;
	GetTerrainPosition(TargetLocation);

}

void ASPlayerPawn::Zoom(float AxisValue)
{
	if(AxisValue==0.f)
	{
		return;
	}

	const float Zoom = AxisValue * 100.f;
	TargetZoom = FMath::Clamp(Zoom + TargetZoom, MinZoom, MaxZoom);
}

void ASPlayerPawn::RotateRight()
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.0f, -45, 0.0f));
}

void ASPlayerPawn::RotateLeft()
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.0f, 45, 0.0f));
}

void ASPlayerPawn::EnableRotate()
{
	CanRotate = true;
}

void ASPlayerPawn::DisableRotate()
{
	CanRotate = false;
}

void ASPlayerPawn::RotateHorizontal(float AxisValue)
{
	if(AxisValue==0.f)
	{
		return;
	}

	if(CanRotate)
	{
		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.f, AxisValue, 0.f));
	}
}

void ASPlayerPawn::RotateVertical(float AxisValue)
{
	if(AxisValue==0.f)
	{
		return;
	}

	if(CanRotate)
	{
		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(AxisValue, 0.f, 0.f));
	}
}

void ASPlayerPawn::EdgeScroll()
{
	if(UWorld* WorldContext = GetWorld())
	{
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(WorldContext);
		const FVector2D ViewPortSize = UWidgetLayoutLibrary::GetViewportSize(WorldContext);

		MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(WorldContext);
		MousePosition.X = MousePosition.X / ViewPortSize.X;
		MousePosition.Y = MousePosition.Y / ViewPortSize.Y;

		// Right/Left

		if(MousePosition.X > 0.98f && MousePosition.X <1.f)
		{
			Right(EdgeScrollSpeed);
		}
		else if(MousePosition.X < 0.02f && MousePosition.X > 0.f)
		{
			Right(-EdgeScrollSpeed);
		}

		// Forward/Backward

		if(MousePosition.Y > 0.98f && MousePosition.Y < 1.f)
		{
			Forward(-EdgeScrollSpeed);
		}
		else if(MousePosition.Y < 0.02f && MousePosition.Y > 0.f)
		{
			Forward(EdgeScrollSpeed);
		}
	}
}

void ASPlayerPawn::CameraBounds()
{
	float NewPitch = TargetRotation.Pitch;
	if(TargetRotation.Pitch < (RotatePitchMax * -1))
	{
		NewPitch = (RotatePitchMax* -1);
	}
	else if(TargetRotation.Pitch > (RotatePitchMax * -1))
	{
		NewPitch = (RotatePitchMin * -1);
	}

	//Set the new pitch, clamp any roll
	TargetRotation = FRotator(NewPitch, TargetRotation.Yaw, 0.f);
}

AActor* ASPlayerPawn::GetSelectedObject()
{
	
}

void ASPlayerPawn::MouseLeftPressed()
{
	
}

void ASPlayerPawn::MouseLeftReleased()
{
	
}

void ASPlayerPawn::MouseRightPressed()
{
	
}

void ASPlayerPawn::MouseRightReleased()
{
	
}

// Called every frame
void ASPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraBounds();

	EdgeScroll();

	//Move the pawn in the desired (interpolated) direction
	const FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, MoveSpeed);
	SetActorLocation(InterpolatedLocation);

	//Zoom the camera in the desired direction
	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, ZoomSpeed);
	SpringArmComponent->TargetArmLength = InterpolatedZoom;

	//Rotate the camera in the desired direction
	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, RotateSpeed);
	SpringArmComponent->SetRelativeRotation(InterpolatedRotation);
	
}

// Called to bind functionality to input
void ASPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ASPlayerPawn::Forward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &ASPlayerPawn::Right);
	PlayerInputComponent->BindAxis(TEXT("Zoom"), this, &ASPlayerPawn::Zoom);
	PlayerInputComponent->BindAxis(TEXT("RotateHorizontal"), this, &ASPlayerPawn::RotateHorizontal);
	PlayerInputComponent->BindAxis(TEXT("RotateVertical"), this, &ASPlayerPawn::RotateVertical);

	PlayerInputComponent->BindAction(TEXT("RotateRight"), IE_Pressed, this, &ASPlayerPawn::RotateRight);
	PlayerInputComponent->BindAction(TEXT("RotateLeft"), IE_Pressed, this, &ASPlayerPawn::RotateLeft);
	PlayerInputComponent->BindAction(TEXT("Rotate"), IE_Pressed, this, &ASPlayerPawn::EnableRotate);
	PlayerInputComponent->BindAction(TEXT("Rotate"), IE_Released, this, &ASPlayerPawn::DisableRotate);

	PlayerInputComponent->BindAction(TEXT("MouseLeft"), IE_Pressed, this, &ASPlayerPawn::MouseLeftPressed);
	PlayerInputComponent->BindAction(TEXT("MouseLeft"), IE_Released, this, &ASPlayerPawn::MouseLeftReleased);
}

