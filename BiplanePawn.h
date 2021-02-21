#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Macros.h"
#include "Curves/RichCurve.h"
#include "Curves/CurveFloat.h"
#include "BiplanePawn.generated.h"

UCLASS()
class MYPROJECT5_API ABiplanePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABiplanePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void ApplyTorque(float InputValue, const FVector& ReferenceAxis);

	bool bThrottleUp;
	bool bThrottleDown;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrottleAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirControl = 1000.f;
	UPROPERTY(EditAnywhere)
	float EnginePower = 1000.f;
	UPROPERTY(EditAnywhere)
	float MaxEnginePower = 25000.f;
	UPROPERTY(EditAnywhere)
	float BankTorqueConst = 80.f;
	UPROPERTY(EditAnywhere)
	float DampingFactor = -1000.f;

	UPROPERTY(EditAnywhere)
	float DampingAngularFactor = -2.f;


	UPROPERTY(EditAnywhere)
	float GroundAngularDrag;

	UPROPERTY(EditAnywhere)
	float AirAngularDrag;

	UPROPERTY(EditAnywhere)
	float NaturalAirThrottle;

	UPROPERTY(EditAnywhere)
	float NaturalGroundThrottle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TopForwardPower = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TopBackwardPower = -20.f;
	
	bool bOnGround;
	bool bInStall;

	UPROPERTY(EditAnywhere, Category = "Curves")
	FRuntimeFloatCurve LiftCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curves")
	FRuntimeFloatCurve EnginePowerCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curves")
	FRuntimeFloatCurve AccelerationCurve;

	FKeyHandle h1;
	FKeyHandle h2;
	
	
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StallAngleOfAttack;
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StallSpeed;
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StallPitchFactor;
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StallRollFactor;
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StallYawFactor;
	

	
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StalingYawTorqueFactor;
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StallingTorqueMagnitude;
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StalingRollTorqueFactor;
	UPROPERTY(EditAnywhere, Category = "Stall values")
	float StallingPitchConstant;

	
	UPROPERTY(EditAnywhere, Category = "Flying values")
	float RollConstant;
	UPROPERTY(EditAnywhere, Category = "Flying values")
	float PitchConstant;
	UPROPERTY(EditAnywhere, Category = "Flying values")
	float YawConstant;


	
	void CalculateThrottle(float InputValue);
	void ApplyEngineDrive();
	void DampenMovement();
	void AddBankingTorque();
	bool InGround();
	void UpdateInGroundControl(float DeltaTime);
	void UpdateStallCondition(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMesh;
	
	void Roll(float InputValue);
	void Yaw(float InputValue);
	void Pitch(float InputValue);
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
