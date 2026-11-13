#pragma once

#include "CoreMinimal.h"
#include "APS.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "APSCharacterMovementComponent.generated.h"



UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Slide			UMETA(DisplayName = "Slide"),
	CMOVE_Hang			UMETA(DisplayName = "Hang"),
	CMOVE_Climb			UMETA(DisplayName = "Climb"),
	CMOVE_WallRun		UMETA(DisplayName = "Wall Run"),
	CMOVE_MAX			UMETA(Hidden),
};


UCLASS()
class  UAPSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	// Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Sprint") float MaxSprintSpeed = 750.f;

	// Slide
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float MinSlideSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float MaxSlideSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float SlideEnterImpulse = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float SlideGravityForce = 4000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float SlideFrictionFactor = .06f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float BrakingDecelerationSliding = 1000.f;

	// Mantle
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleMaxDistance = 200;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleReachHeight = 50;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MinMantleDepth = 30;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleMinWallSteepnessAngle = 75;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleMaxSurfaceAngle = 40;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleMaxAlignmentAngle = 45;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* TallMantleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* TransitionTallMantleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* ShortMantleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* TransitionShortMantleMontage;




	// Hang
	UPROPERTY(EditDefaultsOnly, Category = "Hang") UAnimMontage* TransitionHangMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Hang") UAnimMontage* WallJumpMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Hang") float WallJumpForce = 400.f;

	// Climb
	UPROPERTY(EditDefaultsOnly, Category = "Climb") float MaxClimbSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climb") float BrakingDecelerationClimbing = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climb") float ClimbReachDistance = 200.f;

	// Wall Run
	UPROPERTY(EditDefaultsOnly, Category = "Climbing") float MinWallRunSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climbing") float MaxWallRunSpeed = 800.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climbing") float MaxVerticalWallRunSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climbing") float WallRunPullAwayAngle = 75;
	UPROPERTY(EditDefaultsOnly, Category = "Climbing") float WallAttractionForce = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climbing") float MinWallRunHeight = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climbing") UCurveFloat* WallRunGravityScaleCurve;
	UPROPERTY(EditDefaultsOnly, Category = "Climbing") float WallJumpOffForce = 300.f;

	// Transient
	UPROPERTY(Transient) AAPSCharacter* APSCharacterOwner;

	// Flags
	bool Safe_bWantsToSprint;
	bool Safe_bWantsToProne;
	bool bPressedAPSJump;

	bool Safe_bHadAnimRootMotion;
	bool Safe_bPrevWantsToCrouch;
	bool Safe_bWallRunIsRight;
	bool Safe_bTransitionFinished;

	TSharedPtr<FRootMotionSource_MoveToForce> TransitionRMS;
	FString TransitionName;
	UPROPERTY(Transient) UAnimMontage* TransitionQueuedMontage;
	float TransitionQueuedMontageSpeed;
	int TransitionRMS_ID;


public:
	UAPSCharacterMovementComponent();
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Actor Component
protected:
	virtual void InitializeComponent() override;
	// Character Movement Component
public:
	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;
	/*virtual float GetMaxSpeed() const override;*/
	virtual float GetMaxBrakingDeceleration() const override;

	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves,  float DeltaTime) override;

public:
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
protected:
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	// Vault
public:
	bool TryMantle();
	FVector GetMantleStartLocation(FHitResult FrontHit, FHitResult SurfaceHit, bool bTallMantle) const;

	// Climb
public:
	bool TryHang();

	bool TryClimb();
	void PhysClimb(float deltaTime, int32 Iterations);

	bool HasReachedClimbEdge() const;

	// Slide
public:
	void EnterSlide(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);
	void ExitSlide();
	bool CanSlide() const;
	void PhysSlide(float deltaTime, int32 Iterations);

	//Wall run
public:
	bool TryWallRun();
	void PhysWallRun(float deltaTime, int32 Iterations);

	// Helpers
private:
	float CapR() const;
	float CapHH() const;

	// Interface
public:
	UFUNCTION(BlueprintCallable, Category = "Sprint") void SprintPressed();
	UFUNCTION(BlueprintCallable, Category = "Sprint") void SprintReleased();

	UFUNCTION(BlueprintCallable, Category = "Crouch") void CrouchPressed();
	//UFUNCTION(BlueprintCallable, Category = "Crouch") void CrouchReleased();

	UFUNCTION(BlueprintCallable, Category = "Climb") void ClimbPressed();
	UFUNCTION(BlueprintCallable, Category = "Climb") void ClimbReleased();

	UFUNCTION(BlueprintPure, Category = "Movement") bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	UFUNCTION(BlueprintPure, Category = "Movement") bool IsMovementMode(EMovementMode InMovementMode) const;

	UFUNCTION(BlueprintPure, Category = "Hang") bool IsHanging() const { return IsCustomMovementMode(CMOVE_Hang); }
	UFUNCTION(BlueprintPure, Category = "Climb") bool IsClimbing() const { return IsCustomMovementMode(CMOVE_Climb); }
	UFUNCTION(BlueprintPure, Category = "Climbing") bool IsWallRunning() const { return IsCustomMovementMode(CMOVE_WallRun); }
	UFUNCTION(BlueprintPure, Category = "Climbing") bool WallRunningIsRight() const { return Safe_bWallRunIsRight; }

};