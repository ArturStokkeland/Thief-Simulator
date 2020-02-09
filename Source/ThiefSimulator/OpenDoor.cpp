// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s has OpenDoor component but not pressure plate assigned"), *GetOwner()->GetName());
	}

}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CalculateTotalMassOfActors() > 60.f) {
		OpenDoor(DeltaTime, OpenAngle, DoorOpenSpeed);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else {
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay) {
			OpenDoor(DeltaTime, InitialYaw, DoorCloseSpeed);
		}
	}
	
}

void UOpenDoor::OpenDoor(float DeltaTime, float TargetYaw, int Speed) {
	CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	FRotator OpenDoor(0.f, TargetYaw, 0.f);
	OpenDoor.Yaw = FMath::FInterpTo(CurrentYaw, TargetYaw, DeltaTime, Speed);

	GetOwner()->SetActorRotation(OpenDoor);
}

float UOpenDoor::CalculateTotalMassOfActors() {

	float TotalMass = 0.f;

	if (!PressurePlate) { return TotalMass; }

	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (AActor* Actor : OverlappingActors) {
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;

}
