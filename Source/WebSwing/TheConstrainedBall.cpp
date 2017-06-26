// Fill out your copyright notice in the Description page of Project Settings.

#include "TheConstrainedBall.h"


// Sets default values
ATheConstrainedBall::ATheConstrainedBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MyCollider"));
}

// Called when the game starts or when spawned
void ATheConstrainedBall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATheConstrainedBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

