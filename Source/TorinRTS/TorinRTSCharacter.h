// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Selectable.h"
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
};

