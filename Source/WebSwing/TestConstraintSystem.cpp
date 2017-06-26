// Fill out your copyright notice in the Description page of Project Settings.

#include "TestConstraintSystem.h"


// Sets default values
ATestConstraintSystem::ATestConstraintSystem()
{
	//ThePlayer = Cast<AWebSwingCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	//Player = GetWorld()->GetFirstPlayerController()->GetControlledPawn();
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StableMesh"));
	ConstrainedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConstrainedMesh"));
	ConstrainedMesh->SetSimulatePhysics(true);
	
	TheConstraint = CreateAbstractDefaultSubobject<UPhysicsConstraintComponent>(TEXT("TheConstraint"));
	//TheConstraint->AttachToComponent(StableMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//TheConstraint->SetConstrainedComponents(StableMesh, NAME_None, ConstrainedMesh, NAME_None);
	TheConstraint->SetAngularSwing1Limit(ACM_Limited, 45.f);
	TheConstraint->SetAngularSwing2Limit(ACM_Limited, 45.f);
	TheConstraint->SetAngularTwistLimit(ACM_Free,45.f);
	
}


void ATestConstraintSystem::BeDestroyed() {
	Destroy();
}

// Called when the game starts or when spawned
void ATestConstraintSystem::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,)
	
}

// Called every frame
void ATestConstraintSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

