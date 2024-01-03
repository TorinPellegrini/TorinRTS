// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIData.h"
#include "Selectable.h"
#include "AI/SAIController.h"
#include "GameFramework/Character.h"
#include "TorinRTSCharacter.generated.h"


UCLASS(Blueprintable)
class ATorinRTSCharacter : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	ATorinRTSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
protected:
	UFUNCTION()
	FTransform GetPositionTransform(const FVector Position) const;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	/** Iselectable Interface **/
	virtual void Select() override;
	virtual void DeSelect() override;
	virtual void Highlight(const bool Highlight) override;
	/** End of Iselectable Interface **/

	UPROPERTY()
	bool Selected;

	/**Command Functions **/
public:
	UFUNCTION()
	void CommandMoveToLocation(const FCommandData CommandData);

	UFUNCTION()
	void SetAIController(ASAIController* NewAIController) { SAIController = NewAIController; }

protected:
	UFUNCTION()
	void CommandMove(const FCommandData CommandData);

	UFUNCTION()
	void DestinationReached(const FCommandData CommandData);

	UFUNCTION()
	void SetWalk() const;

	UFUNCTION()
	void SetRun() const;

	UFUNCTION()
	void SetSprint() const;

	UFUNCTION()
	void SetOrientation(const float DeltaTime);

	UFUNCTION()
	bool IsOriented() const;

	UFUNCTION()
	void SetMoveMarker(const FVector Location);

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY()
	float MaxSpeed;
	
	UPROPERTY()
	FRotator TargetOrientation;

	UPROPERTY()
	uint8 ShouldOrientate;

	UPROPERTY()
	ASAIController* SAIController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Settings")
	TSubclassOf<AActor> MoveMarkerClass;

	UPROPERTY()
	AActor* MoveMarker;
	
	
	/** End Command Functions **/
};

