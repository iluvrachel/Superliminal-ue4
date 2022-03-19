// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "ProceduralMeshComponent.h"
#include "RescaleComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPERLIMINAL_API URescaleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URescaleComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// UFUNTION(BlueprintCallable, Category = "GetHit");
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Grab(); //Grab an actor
	float HitDistance;
	void Release();
	UInputComponent* inputComponent = nullptr;
	FHitResult hit;
	FHitResult GetHitResult();
	FVector TargetPosition(FHitResult hit); // Get Line hit position

	// data structure for actor informations
	UStaticMesh * InMesh = nullptr;
	// TArray<FVector>& Vertices;

	TArray<FVector> GetAllVertices(FHitResult hit);
	float CastRayToAll(TArray<FVector> Vertices);

	void Rescale(FHitResult hit);

		
};


class UPhysicsHandleComponent;
UPhysicsHandleComponent* physicsHandel = nullptr;