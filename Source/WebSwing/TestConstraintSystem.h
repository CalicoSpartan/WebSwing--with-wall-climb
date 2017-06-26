// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "WebSwingCharacter.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "TestConstraintSystem.generated.h"

UCLASS()
class WEBSWING_API ATestConstraintSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestConstraintSystem();

	UFUNCTION()
		void BeDestroyed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StableMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* ConstrainedMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPhysicsConstraintComponent* TheConstraint;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class AWebSwingCharacter* ThePlayer;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//AWebSwingCharacter* Player;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
