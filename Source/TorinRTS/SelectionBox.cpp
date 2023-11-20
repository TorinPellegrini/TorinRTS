// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectionBox.h"

// Sets default values
ASelectionBox::ASelectionBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASelectionBox::Start(FVector Position, const FRotator Rotation)
{
}

void ASelectionBox::End()
{
}

// Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

