// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine.h" //tutorial
#include "TheConstrainedBall.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Components/MeshComponent.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "TestConstraintSystem.h"
#include "Components/StaticMeshComponent.h"

#include "WebSwingCharacter.generated.h"

UCLASS(config=Game)
class AWebSwingCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AWebSwingCharacter();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* ConstraintLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* WebStart;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//UStaticMeshComponent* WebStartMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* WebEndPoint;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UParticleSystemComponent* WebParticles;

	UFUNCTION()
		void OnShoot(); //tutorial
	UFUNCTION()
		void OnRelease(); //tutorial
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanShoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsWebbed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SwingReleaseBoost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float distancetoplayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector SpawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ATestConstraintSystem* thisSystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool LerpingRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ATheConstrainedBall* thisBall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ATheConstrainedBall> TestBall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ATestConstraintSystem> TestSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float JumpCameraLength; //tutorial
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FHitResult hit; //tutorial
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FHitResult GroundTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FHitResult BuildingCheckHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ATestConstraintSystem*> ConstraintSystems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ControllerYaw;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	virtual void Tick(float DeltaSeconds) override; //function is called every frame //tutorial
	virtual void BeginPlay();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

