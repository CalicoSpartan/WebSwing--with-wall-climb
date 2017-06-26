// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "WebSwingCharacter.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AWebSwingCharacter

AWebSwingCharacter::AWebSwingCharacter()
{
	PrimaryActorTick.bCanEverTick = true; 
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	WebStart = CreateDefaultSubobject<USceneComponent>(TEXT("WebStart"));
	ConstraintLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ConstraintLocation"));
	//WebStartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WebStartMesh"));
	//WebStartMesh->SetupAttachment(WebStart);
	CanShoot = true;
	distancetoplayer = 0.f;
	ControllerYaw = 0.f;
	IsWebbed = false;
	JumpCameraLength = 600;
	WebEndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WebEndPoint"));
	WebParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("WebParticles"));
	SwingReleaseBoost = 400.f;
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}



//////////////////////////////////////////////////////////////////////////
// Input

void AWebSwingCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AWebSwingCharacter::OnShoot); //tutorial
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AWebSwingCharacter::OnRelease); //tutorial

	PlayerInputComponent->BindAxis("MoveForward", this, &AWebSwingCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWebSwingCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWebSwingCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWebSwingCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AWebSwingCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AWebSwingCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AWebSwingCharacter::OnResetVR);
}


void AWebSwingCharacter::BeginPlay() {
	Super::BeginPlay();
	thisBall = GetWorld()->SpawnActor<ATheConstrainedBall>(TestBall, AWebSwingCharacter::GetActorLocation(), FRotator(0.f));



}

//tutorial
void AWebSwingCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);


	/****************************this is all for checking if player is facing in direction of velocity (unneeded) *********************

	FVector2D myforward2d = FVector2D(AWebSwingCharacter::GetActorForwardVector().X, AWebSwingCharacter::GetActorForwardVector().Y).GetSafeNormal();
	FVector2D myworldvelocity2d = FVector2D(ACharacter::GetVelocity().X, ACharacter::GetVelocity().Y).GetSafeNormal();
	float dotforward = FVector2D::DotProduct(myforward2d, myworldvelocity2d);
	if (dotforward >= .7f) {
		//GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Cyan, "MovingForward");
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Cyan, "NotMovingForward");
	}
	//GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Cyan, ACharacter::GetVelocity().GetSafeNormal().ToString());
	*/





	/*****************          Jump Camera        *******************/
	if (GetCharacterMovement()->IsFalling() == true) {
		float ArmLength = FMath::Lerp(GetCameraBoom()->TargetArmLength, JumpCameraLength, 4 * DeltaTime);
		GetCameraBoom()->TargetArmLength = ArmLength;
	}
	else {
		float ArmLength = FMath::Lerp(GetCameraBoom()->TargetArmLength, 300.f, 4 * DeltaTime);
		GetCameraBoom()->TargetArmLength = ArmLength;
	}
	/********************************************************************************/

	/*********************************** If the player is swinging ********************************************/

	if (IsWebbed == true) {
		AWebSwingCharacter::SetActorLocation(thisBall->GetActorLocation());


		WebEndPoint->SetWorldLocation(hit.Location);

		if (ACharacter::GetVelocity().Z < 0) {
			thisBall->MyCollider->AddForce(thisBall->GetVelocity() * SwingReleaseBoost); //this boosts the player as the player moves down
		}
		WebParticles->SetBeamSourcePoint(0, WebStart->GetComponentLocation(), 0);
		WebParticles->SetBeamTargetPoint(0, WebEndPoint->GetComponentLocation(), 0);
		if (AWebSwingCharacter::GetActorLocation().Z > thisSystem->TheConstraint->GetComponentLocation().Z + 20) {
			OnRelease();
		}
		
		
	}

	/***************************************************************************************************************/


	//if we are grounded, ball attaches to us, else, we attach to ball

	if (IsWebbed == false && thisBall->GetVelocity().Z == 0){ 
		thisBall->SetActorLocation(AWebSwingCharacter::GetActorLocation());

	}
	else {
		AWebSwingCharacter::SetActorLocation(thisBall->GetActorLocation());
	}

////////////////////////////////////////////////////////////////////////////////////////


//this is another check to see if we are on the ground (just to make sure)
	if (ACharacter::GetCharacterMovement()->IsFalling() == false) {
		thisBall->MyCollider->SetSimulatePhysics(false);
		thisBall->SetActorLocation(AWebSwingCharacter::GetActorLocation());
	}
	else {

	}
/////////////////////////////////////////////////////////////////////////////////
	


/*********** Check if we are about to hit ground so we can change some things *********************************************/	
	FCollisionQueryParams MyParams(FName(TEXT("GroundTrace")), false, this);
	MyParams.AddIgnoredActor(thisBall);
	if ((ACharacter::GetVelocity().Z <= 0 || thisBall->GetVelocity().Z <= 0) && ACharacter::GetCharacterMovement()->IsMovingOnGround() == false && (GetWorld()->LineTraceSingleByChannel(GroundTrace, AWebSwingCharacter::GetActorLocation(), AWebSwingCharacter::GetActorLocation()+ AWebSwingCharacter::GetActorUpVector() * -110, ECC_Visibility, MyParams)))
	{

		thisBall->MyCollider->SetSimulatePhysics(false);


		thisBall->SetActorLocation(AWebSwingCharacter::GetActorLocation());
	}

/***********************************************************************************************************************/



}






void AWebSwingCharacter::OnRelease()
{	
	/**********************   breaks us away from constraint when we release *********************************************/
	if (IsWebbed == true) {
		thisSystem->TheConstraint->BreakConstraint();
		WebParticles->Deactivate();
		CanShoot = true;
		IsWebbed = false;		
	}	
}




void AWebSwingCharacter::OnShoot()
{
	/****************************************** Shooting web **************************************************************/
	if (CanShoot == true && IsWebbed == false && ACharacter::GetVelocity().Z <= 0 && AWebSwingCharacter::GetCharacterMovement()->IsFalling() == true) {
		FCollisionQueryParams MyParams(FName(TEXT("WebTrace")), false, this);
		if (GetWorld()->LineTraceSingleByChannel(hit, AWebSwingCharacter::GetActorLocation(), FollowCamera->GetForwardVector() * 100000, ECC_Visibility,MyParams) == true) {
			if (hit.GetActor()->ActorHasTag("CanWeb") && hit.Location.Z > AWebSwingCharacter::GetActorLocation().Z) {
				FVector MyLocation = AWebSwingCharacter::GetActorLocation();
				IsWebbed = true;
				CanShoot = false;
				WebEndPoint->SetWorldLocation(hit.Location);
				WebParticles->Activate();

				/*************************   Creates constraint and attaches player  *****************************/
				if (TestSystem != NULL) {
					//FHitResult hitfromplayer;
					//if (GetWorld()->LineTraceSingleByChannel(hitfromplayer,AWebSwingCharacter::GetActorLocation(),hit.Location, ECC_Visibility, MyParams) == true)
					FVector2D totaldirection = FVector2D(hit.Location.X - MyLocation.X, hit.Location.Y - MyLocation.Y);
					
					FVector2D unitdirection = totaldirection.SafeNormal();// * (totaldirection.Size() / 4);
					
					//FVector2D totaldirection = FVector2D(hit.Location.X - MyLocation.X, hit.Location.X - MyLocation.Y);
					//FVector2D unitdirection = totaldirection.GetSafeNormal();// * (totaldirection.Size() / 4);
					FHitResult directhit;
					DrawDebugLine(GetWorld(), FVector(hit.Location.X, hit.Location.Y, hit.Location.Z) - FVector(unitdirection.X, unitdirection.Y, 0), FVector(hit.Location.X, hit.Location.Y, hit.Location.Z) - FVector(unitdirection.X, unitdirection.Y, 0) + (hit.Normal * -1.f * 10000.f), FColor::Green, true);
					if (GetWorld()->LineTraceSingleByChannel(directhit, FVector(hit.Location.X, hit.Location.Y, hit.Location.Z) - FVector(unitdirection.X, unitdirection.Y, 0), FVector(hit.Location.X, hit.Location.Y, hit.Location.Z) - FVector(unitdirection.X, unitdirection.Y, 0) + (hit.Normal * -1.f * 10000.f), ECollisionChannel::ECC_Visibility, MyParams)) {
						unitdirection = totaldirection.GetSafeNormal() * (totaldirection.Size() / 4);
						float unitdist = directhit.Distance;
						float fulldist = unitdist * totaldirection.Size()/2;
						DrawDebugLine(GetWorld(), MyLocation, MyLocation + (-1.f * hit.Normal * fulldist), FColor::Blue, true);
						SpawnPoint = hit.Location + FVector(hit.Normal.X * fulldist, hit.Normal.Y * fulldist, fulldist);
						GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, "Start:  " + FVector(FVector(hit.Location.X, hit.Location.Y, hit.Location.Z) + FVector(unitdirection.X, unitdirection.Y, 0)).ToString());
						GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, "unit distance:   " + FString::SanitizeFloat(unitdist));
						GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, "End:  " + FVector(FVector(hit.Location.X, hit.Location.Y, hit.Location.Z) + FVector(unitdirection.X, unitdirection.Y, 0) + (hit.Normal * -1.f * 10000.f)).ToString());
						//DrawDebugPoint(GetWorld(), middle, 5, FColor::Green, true);
					}
					//DrawDebugLine(GetWorld(), AWebSwingCharacter::GetActorLocation(), AWebSwingCharacter::GetActorLocation() + hit.Normal * (hit.Location.X - AWebSwingCharacter::GetActorLocation().X) * -1.f,FColor::Red,true);
					//DrawDebugPoint(GetWorld(), AWebSwingCharacter::GetActorLocation() + FVector(hit.Location.X - AWebSwingCharacter::GetActorLocation().X, hit.Location.Y - AWebSwingCharacter::GetActorLocation().Y, hit.Location.Z - AWebSwingCharacter::GetActorLocation().Z), 50, FColor::Green, true);
					//GEngine->AddOnScreenDebugMessage(-1, 30, FColor::Red, FVector2D(hit.Location.X - AWebSwingCharacter::GetActorLocation().X, hit.Location.Y - AWebSwingCharacter::GetActorLocation().Y).ToString());
					float distanceto = AWebSwingCharacter::GetDistanceTo(hit.GetActor()) / 3;
					//SpawnPoint = hit.Location + FVector(hit.Normal.X * distanceto, hit.Normal.Y * distanceto, distanceto);
					thisSystem = GetWorld()->SpawnActor<ATestConstraintSystem>(TestSystem, SpawnPoint, hit.Normal.Rotation());
					thisSystem->StableMesh->SetWorldLocation(SpawnPoint);
					thisSystem->TheConstraint->SetWorldLocation(SpawnPoint);
					ConstraintSystems.Add(thisSystem);
					thisBall->MyCollider->SetSimulatePhysics(true);
					thisBall->MyCollider->SetAllPhysicsLinearVelocity(GetCapsuleComponent()->GetPhysicsLinearVelocity());

					thisSystem->StableMesh->SetMobility(EComponentMobility::Stationary);
					thisSystem->TheConstraint->AttachToComponent(thisSystem->StableMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					thisSystem->TheConstraint->SetConstrainedComponents(thisSystem->StableMesh, NAME_None, thisBall->MyCollider, NAME_None);

				}

			}
		}

	}


}




void AWebSwingCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AWebSwingCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AWebSwingCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AWebSwingCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWebSwingCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWebSwingCharacter::MoveForward(float Value)
{
	
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

}

void AWebSwingCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
