// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Components/ShapeComponent.h"
#include "TheConstrainedBall.generated.h"

UCLASS()
class WEBSWING_API ATheConstrainedBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATheConstrainedBall();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UShapeComponent* MyCollider;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
