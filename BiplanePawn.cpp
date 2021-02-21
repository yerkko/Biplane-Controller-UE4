#include "BiplanePawn.h"

#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Actor.h"


// Sets default values
ABiplanePawn::ABiplanePawn()
{
	//Mesh and physics
	
	PrimaryActorTick.bCanEverTick = true;
	ThrottleAmount = 0.f;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMesh->SetSimulatePhysics(true);
	
	// Lift curves

	LiftCurve.EditorCurveData.AddKey(0.f, 0.f);
	LiftCurve.EditorCurveData.AddKey(100.f, 1.f);

	LiftCurve.EditorCurveData.AddKey(0.f, 0.f);
	LiftCurve.EditorCurveData.AddKey(0.5f, 0.5f);
	LiftCurve.EditorCurveData.AddKey(1.f, 1.f);

}

// Called when the game starts or when spawned
void ABiplanePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABiplanePawn::ApplyTorque(const float InputValue, const FVector& ReferenceAxis)
{
	const float FinalTorque = InputValue * AirControl;
	const FVector TorqueVector = FMath::Lerp(FVector::ZeroVector, ReferenceAxis * FinalTorque, 0.1);
	StaticMesh->AddTorqueInDegrees(TorqueVector, NAME_None, true);
}

void ABiplanePawn::Roll(float InputValue)
{
	auto rollValue = InputValue * RollConstant;
	if(bInStall) rollValue *= StallRollFactor;
	ApplyTorque(rollValue, GetActorForwardVector());
}

void ABiplanePawn::Yaw(float InputValue)
{
	auto yawValue = InputValue * YawConstant;
	if(bInStall) yawValue *= StallYawFactor;
	ApplyTorque(yawValue, GetActorUpVector());
}

void ABiplanePawn::Pitch(float InputValue)
{
	auto pitchValue = InputValue * PitchConstant;
	if(bInStall) pitchValue *= StallPitchFactor;
	ApplyTorque(pitchValue, GetActorRightVector());
}




void ABiplanePawn::CalculateThrottle(float InputValue)
{
	float targetThrottle;
	float acceleration = AccelerationCurve.EditorCurveData.Eval(FMath::Abs(ThrottleAmount));	
	bThrottleUp = !FMath::IsNearlyZero(InputValue) && FMath::Sign(InputValue) == 1;

	if(bThrottleUp)
	{
		targetThrottle = EnginePowerCurve.EditorCurveData.Eval(InputValue);

	}
	else
	{
		targetThrottle = bOnGround ? NaturalGroundThrottle : NaturalAirThrottle;		
	}

	
	ThrottleAmount = FMath::Clamp(FMath::Lerp(ThrottleAmount, targetThrottle, 0.01f * acceleration), TopBackwardPower, TopForwardPower);
	
}




// Called to bind functionality to input
void ABiplanePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("ThrottleUp", this, &ABiplanePawn::CalculateThrottle);
	PlayerInputComponent->BindAxis("Roll", this, &ABiplanePawn::Roll);
	PlayerInputComponent->BindAxis("Yaw", this, &ABiplanePawn::Yaw);
	PlayerInputComponent->BindAxis("Pitch", this, &ABiplanePawn::Pitch);

}


void ABiplanePawn::ApplyEngineDrive()
{
	auto currentVelocity = StaticMesh->GetPhysicsLinearVelocity();
	auto forward = GetActorForwardVector();
	auto targetVelocity =  forward * FMath::Clamp(ThrottleAmount * EnginePower, 0.f, MaxEnginePower);
	auto newVelocity = FMath::Lerp(currentVelocity, targetVelocity, 0.01);

	if(bInStall)
	{
		auto stallTargetVelocity = FVector::ZeroVector;
		newVelocity -=  FMath::Lerp(currentVelocity,stallTargetVelocity,0.01);
		auto stallPitch = FVector::CrossProduct(forward, FVector(0,0,1)).Size();
		Pitch(stallPitch * StallingPitchConstant);
	}

	
	StaticMesh->SetPhysicsLinearVelocity(newVelocity, false, NAME_None);

	auto weight = StaticMesh->GetMass() * abs(GetWorld()->GetGravityZ());
	auto LiftForce = LiftCurve.EditorCurveData.Eval(ThrottleAmount) * weight * 2.f;
	if(bOnGround)
	{
		StaticMesh->AddForce(FVector(0,0,LiftForce));
	}
	
}

void ABiplanePawn::DampenMovement()
{
	StaticMesh->AddTorqueInDegrees(DampingAngularFactor * StaticMesh->GetPhysicsAngularVelocityInDegrees(), NAME_None, true);
	StaticMesh->AddForce(FVector(0, 0, DampingFactor), NAME_None, true);
}

void ABiplanePawn::AddBankingTorque()
{
	auto up = FVector::UpVector;
	auto bankFactor = -1.f * GetActorRightVector().Z;
	StaticMesh->AddTorqueInDegrees(up * (bankFactor * BankTorqueConst), NAME_None, true);
	
}

bool ABiplanePawn::InGround()
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	const auto TraceStart = GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const auto TraceEnd = GetActorLocation() + FVector(0.f, 0.f, -200.f);

	FHitResult Hit;

	//The ray cast hit the ground 
	const bool bInGround = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	return bInGround;
}


void ABiplanePawn::UpdateInGroundControl(float DeltaTime)
{

	//If we're not in the air, use ground controls
	
	bOnGround = InGround();
	if(bOnGround)
	{
		StaticMesh->SetAngularDamping(GroundAngularDrag);
		StaticMesh->BodyInstance.bLockYRotation = true;

	}
	else
	{
		StaticMesh->SetAngularDamping(AirAngularDrag);
		StaticMesh->BodyInstance.bLockYRotation = false;

	}
	
	
}

void ABiplanePawn::UpdateStallCondition(float DeltaTime)
{
	auto forward = GetActorForwardVector().Z;
	auto newStall = forward >= StallAngleOfAttack && StaticMesh->GetPhysicsLinearVelocity().Size() < StallSpeed;
	if(newStall && !bInStall)
	{
		
		ApplyTorque(FMath::RandRange(-StalingYawTorqueFactor, StalingYawTorqueFactor ) *  StallingTorqueMagnitude, GetActorUpVector());
		ApplyTorque(FMath::RandRange(-StalingRollTorqueFactor, StalingRollTorqueFactor ) *  StallingTorqueMagnitude, GetActorUpVector());
		// UE_LOG(LogTemp, Warning, TEXT("Ship entered stall"));	
	}
	bInStall = newStall;
	
	
}

void ABiplanePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInGroundControl(DeltaTime);
	UpdateStallCondition(DeltaTime);
	ApplyEngineDrive();
	DampenMovement();
	AddBankingTorque();
	
	
}
