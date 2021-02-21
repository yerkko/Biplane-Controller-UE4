#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BiplanePawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "LandingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT5_API ULandingComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULandingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	bool ShouldStartLandingSequence();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AngleOfAttackThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RollRotationDampSpeed = 4.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakeHelper;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PitchRotationDampSpeed = 197.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TouchDownHelpingDistance = 50.f;

	ABiplanePawn* AttachedActor;
	
	bool bIsLandingGearActive;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCurrentlyLanding;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LandingHelpingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LandingDamping;

	
	float CurrentDistanceToGround;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OnLandDistanceThreshold;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool StartLandingGear();
	void DampRotation(float DeltaTime);
	void DampSpeed(float DeltaTime);
	void TouchDownRotationAdjustment();
	void GroundSpeedRotationAdjustment();
};
