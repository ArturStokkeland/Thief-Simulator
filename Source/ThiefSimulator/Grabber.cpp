// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Math/Color.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{

	Super::BeginPlay();

	GetPhysicsHandle();
	GetInputComponent();

}

void UGrabber::GetPhysicsHandle() {

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s Did not initialize with a PhysicsHandle"), *GetOwner()->GetName());
	}

}

void UGrabber::GetInputComponent() {

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s Did not initialize with a InputComponent"), *GetOwner()->GetName());
	}

}

void UGrabber::Grab() {

	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	if (HitResult.GetActor()) {
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetLineTraceEnd()
		);
	}

}

void UGrabber::Release() {

	PhysicsHandle->ReleaseComponent();

}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() {

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetViewPointLocation(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;

}

FVector UGrabber::GetViewPointLocation() {

	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewLocation, OUT PlayerViewRotation);

	return PlayerViewLocation;

}

FVector UGrabber::GetLineTraceEnd() {
	
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewLocation, OUT PlayerViewRotation);

	return PlayerViewLocation + PlayerViewRotation.Vector() * Reach;

}
