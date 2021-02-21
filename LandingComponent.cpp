// Fill out your copyright notice in the Description page of Project Settings.


#include "LandingComponent.h"

// Sets default values for this component's properties
ULandingComponent::ULandingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	AttachedActor = Cast<ABiplanePawn>(GetAttachmentRootActor());



	// ...
}


// Called when the game starts
void ULandingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool ULandingComponent::ShouldStartLandingSequence(){


	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AttachedActor);
	const FVector location = AttachedActor->GetActorLocation();
	const FVector TraceStart = location + FVector(0.f,0.f,50.f);

	const FVector TraceEnd = location + FVector (0.f, 0.f, -LandingHelpingDistance);

	FHitResult Hit;

	const bool bShouldStartLanding = GetWorld() -> LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	auto hitLocation = Hit.Location;

	
	bool bIsRunway = false;

	if(IsValid(Hit.GetActor()))
	{
		bIsRunway = Hit.GetActor()->ActorHasTag("Runway");
	}

	CurrentDistanceToGround = Hit.Distance;

	return bShouldStartLanding && bIsRunway;
}

bool ULandingComponent::StartLandingGear(){
	bIsLandingGearActive = !bIsLandingGearActive;

/*
	TODO(Do intermediate routine, like firing animation signals and hud elements)
*/

	return bIsLandingGearActive;
}


void ULandingComponent::DampRotation(float DeltaTime){

	// Get rotation measures
	
	float LandingPitch = FVector::DotProduct(AttachedActor->GetActorUpVector(), FVector(0,0,1));
	float LandingRollAngle = AttachedActor->GetActorRotation().Euler().X;

	// Calculate rotational values
	
	float RollingValue = LandingRollAngle < 0 ? FMath::Abs(LandingRollAngle) : -LandingRollAngle;
	float PitchValue = (1 - LandingPitch);

	//Adjust to game state
	
	RollingValue *= DeltaTime * RollRotationDampSpeed;
	PitchValue *= DeltaTime * PitchRotationDampSpeed;

	
	AttachedActor->Roll(FMath::Abs(LandingRollAngle) > 10.f ? -RollingValue: 0.f);
	AttachedActor->Pitch( PitchValue );


}

void ULandingComponent::DampSpeed(float DeltaTime){

}



void ULandingComponent::TouchDownRotationAdjustment(){

	AttachedActor->StaticMesh->BodyInstance.bLockYRotation = true;

}

void ULandingComponent::GroundSpeedRotationAdjustment(){

}




// Called every frame
void ULandingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bCurrentlyLanding = ShouldStartLandingSequence();

	if(bCurrentlyLanding)
	{
		DampRotation(DeltaTime);
		if(CurrentDistanceToGround < TouchDownHelpingDistance)
		{
		TouchDownRotationAdjustment();
		}
		if(CurrentDistanceToGround < OnLandDistanceThreshold){
			GroundSpeedRotationAdjustment();
		}
	}

}

