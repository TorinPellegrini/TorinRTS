// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "SelectionBox.h"
#include "SPlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "SPlayerPawn.generated.h"


class ASelectionBox;
class ASPlayerController;
class UCameraComponent;
class USpringArmComponent;
UCLASS()
class TORINRTS_API ASPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASPlayerPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void GetTerrainPosition(FVector& TerrainPosition) const;

	UFUNCTION()
	void EdgeScroll();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MoveSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float EdgeScrollSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotateSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotatePitchMin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotatePitchMax;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float ZoomSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MinZoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MaxZoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float Smoothing;

private:
	UFUNCTION()
	void CameraBounds();
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	UCameraComponent* CameraComponent;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	float TargetZoom;

	/** Mouse Input **/
protected:
	UFUNCTION()
	AActor* GetSelectedObject();

	UFUNCTION()
	void CreateSelectionBox();

	UPROPERTY()
	ASPlayerController* SPlayer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	TSubclassOf<ASelectionBox> SelectionBoxClass;

	UPROPERTY()
	ASelectionBox* SelectionBox;

	UPROPERTY()
	bool BoxSelect;

	UPROPERTY()
	FVector SelectHitLocation;

	/** Enhanced Input **/
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Rotate(const FInputActionValue& Value);

	UFUNCTION()
	void Select(const FInputActionValue& Value);

	UFUNCTION()
	void SelectHold(const FInputActionValue& Value);

	UFUNCTION()
	void SelectEnd(const FInputActionValue& Value);

	UFUNCTION()
	void Zoom(const FInputActionValue& Value);

	UFUNCTION()
	void TestPlacement(const FInputActionValue& Value);

	UFUNCTION()
	void SelectDoubleTap(const FInputActionValue& Value);
	
	//Placement Controls
	UFUNCTION()
	void Place(const FInputActionValue& Value);

	UFUNCTION()
	void PlaceCancel(const FInputActionValue& Value);

	//Modifier Keys
	UFUNCTION()
	void Shift(const FInputActionValue& Value);

	UFUNCTION()
	void Alt(const FInputActionValue& Value);

	UFUNCTION()
	void Ctrl(const FInputActionValue& Value);
	
	//Shift
	UFUNCTION()
	void ShiftSelect(const FInputActionValue& Value);

	//Alt
	UFUNCTION()
	void AltSelect(const FInputActionValue& Value);

	UFUNCTION()
	void AltSelectEnd(const FInputActionValue& Value);

	//Ctrl
	UFUNCTION()
	void CtrlSelect(const FInputActionValue& Value);

	UFUNCTION()
	void CtrlSelectEnd(const FInputActionValue& Value);
};
