// Fill out your copyright notice in the Description page of Project Settings.


#include "RescaleComponent.h"

#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "DrawDebugHelpers.h"
//#include "Engine/StaticMesh.h"
//#include "StaticMeshResources.h"

// Sets default values for this component's properties
URescaleComponent::URescaleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URescaleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	inputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	physicsHandel = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();




	if (nullptr != inputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("find inputComponent"));
		//inputComponent->BindAction("Grab", IE_Pressed, this, &URescaleComponent::Grab);
		inputComponent->BindAction("Grab", IE_Pressed, this, &URescaleComponent::Grab);
		inputComponent->BindAction("Grab", IE_Released, this, &URescaleComponent::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("not find inputComponent"));
	}
}


// Called every frame
void URescaleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (physicsHandel && physicsHandel->GrabbedComponent)
	{
		//physicsHandel->SetTargetLocation(LineTrace());
		URescaleComponent::Rescale(URescaleComponent::hit);
		//physicsHandel->SetTargetLocation(TargetPosition(URescaleComponent::hit)); // todo
		
	}
}


void URescaleComponent::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("inputComponent press"));
	URescaleComponent::hit = GetHitResult();

	FVector CameraLocation;
	CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	//URescaleComponent::GrabDistance = (hit.Location - CameraLocation).Size();
	UPrimitiveComponent* primit = hit.GetComponent();
	if (hit.GetActor() && physicsHandel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grab"));
		physicsHandel->GrabComponentAtLocationWithRotation(primit, NAME_None, primit->GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());

		// get all vertices of grabbing object
		// TArray<FVector> test = GetAllVertices(hit);
		//NormalizeScale();
	}
}

void URescaleComponent::Release()
{
	UE_LOG(LogTemp, Error, TEXT("inputComponent release"));
	if (physicsHandel)
	{
		physicsHandel->ReleaseComponent();
	}
	//Rescale();
}

FHitResult URescaleComponent::GetHitResult()
{
	FVector CamLoc;
	FRotator CamRot;
	FVector TraceEnd;
	//FHitResult HitResult(ForceInit);
	TArray< struct FHitResult > HitResult;
	FHitResult FirstHit(ForceInit);
	if (this->GetWorld()->GetFirstPlayerController() == NULL)
		return FirstHit;


	this->GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CamLoc, CamRot);
	APlayerController* pc = GetWorld()->GetFirstPlayerController();

	if (pc)
	{
		ULocalPlayer* localplayer = pc->GetLocalPlayer();
		if (localplayer && localplayer->ViewportClient)
		{
			FVector Dir, point;
			if (pc->DeprojectMousePositionToWorld(point, Dir))
			{
				FVector TraceStart = CamLoc;
				FVector Direction = CamRot.Vector();
				//detect line length
				TraceEnd = TraceStart + (Direction * 1000);

				FString temp = FString(TEXT("Actor"));
				FCollisionQueryParams  TraceParams(FName(*temp), true, this->GetOwner());
				//FCollisionQueryParams TraceParams = new FCollisionQueryParams();

				//TraceParams.bTraceAsyncScene = true;
				TraceParams.bReturnPhysicalMaterial = false;
				TraceParams.bTraceComplex = true;


				GetWorld()->LineTraceMultiByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
				if (HitResult.Num() > 0)
				{
					GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, FString::FromInt(HitResult.Num()));
					FirstHit = HitResult[0];
					if (Cast<AActor>(FirstHit.GetActor()))
					{
						//GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, FirstHit.GetActor()->GetName());
					}

				}
				//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

			}
		}
	}
	return FirstHit;
}


FVector URescaleComponent::TargetPosition(FHitResult HitResult)
{
	FVector CamLoc;
	FRotator CamRot;
	FVector TraceEnd;

	FVector TargetEnd;

	// float HitDistance; // the object distance from cam

	if (this->GetWorld()->GetFirstPlayerController() == NULL)
		return TraceEnd;
	this->GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CamLoc, CamRot);
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	FVector Direction;
	if (pc)
	{
		ULocalPlayer* localplayer = pc->GetLocalPlayer();
		if (localplayer && localplayer->ViewportClient)
		{

			FVector TraceStart = CamLoc;
			Direction = CamRot.Vector();


			URescaleComponent::HitDistance = (CamLoc - URescaleComponent::hit.GetActor()->GetActorLocation()).Size();
			TargetEnd = TraceStart + (Direction * URescaleComponent::HitDistance);
		}
	}
	return TargetEnd;
}

// 获取mesh每个顶点
// 计算每个顶点的目标位置for raycast
//TArray<FVector> URescaleComponent::GetAllVertices(FHitResult HitResult)
//{
//	// Get static mesh
//	TArray<UStaticMeshComponent*> Comp;
//	URescaleComponent::hit.GetActor()->GetComponents<UStaticMeshComponent>(Comp);
//	URescaleComponent::InMesh = Comp[0]->GetStaticMesh();
//
//	// Get Vertices
//	TArray<FVector> Vertices ;
//	TArray<int32> Triangles;
//	TArray<FVector> Normals;
//	TArray<FVector2D> UVs;
//	TArray<FProcMeshTangent> Tangents;
//	UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(InMesh,0,0,Vertices,Triangles,Normals,UVs,Tangents);
//
//	UE_LOG(LogTemp, Log, TEXT("Vertices is %d"), Vertices.Num());
//	//GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Green, URescaleComponent::hit.GetActor()->GetName());
//	
//	return Vertices;
//}


TArray<FVector> URescaleComponent::GetAllVertices(FHitResult HitResult)
{
	TArray<UStaticMeshComponent*> Comp;
	HitResult.GetActor()->GetComponents<UStaticMeshComponent>(Comp);

	TArray<FVector> vertices = TArray<FVector>();

	// Vertex Buffer
	if (!IsValidLowLevel()) return vertices;
	if (!Comp[0]) return vertices;
	if (!Comp[0]->GetStaticMesh()) return vertices;
	if (!Comp[0]->GetStaticMesh()->RenderData) return vertices;
	if (Comp[0]->GetStaticMesh()->RenderData->LODResources.Num() > 0)
	{
		FPositionVertexBuffer* VertexBuffer = &Comp[0]->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
		if (VertexBuffer)
		{
			const int32 VertexCount = VertexBuffer->GetNumVertices();
			for (int32 Index = 0; Index < VertexCount; Index++)
			{
				//This is in the Static Mesh Actor Class, so it is location and tranform of the SMActor
				const FVector WorldSpaceVertexLocation = HitResult.GetActor()->GetActorLocation() + HitResult.GetActor()->GetTransform().TransformVector(VertexBuffer->VertexPosition(Index));
				//add to output FVector array
				vertices.Add(WorldSpaceVertexLocation);
			}
		}
	}

	return vertices;
}

float URescaleComponent::CastRayToAll(TArray<FVector> Vertices)
{
	//TArray< struct FHitResult > AllHitResult;
	TArray< float > AllHitDistance;
	float TargetDistance = FLT_MAX;
	int NearestVertice = 0;
	FVector CamLoc;
	FRotator CamRot;
	this->GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CamLoc, CamRot);
	//FVector TraceStart = CamLoc;

	//设置碰撞参数
	FString temp = FString(TEXT("Actor"));
	FCollisionQueryParams  TraceParams(FName(*temp), true, this->GetOwner());
	//FCollisionQueryParams TraceParams = new FCollisionQueryParams();

	//TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true; // 碰撞精确到面

	for (size_t i = 0; i < Vertices.Num(); i++)
	{
		//DrawDebugPoint(GetWorld(), Vertices[i], 20, FColor::Orange,false,10);
		TArray< struct FHitResult > HitResult;
		// we need the second one
		FVector TraceEnd = CamLoc + (Vertices[i]-CamLoc)* 10000;
		GetWorld()->LineTraceMultiByChannel(HitResult, CamLoc, TraceEnd, ECC_Visibility, TraceParams);
		DrawDebugLine(GetWorld(),CamLoc, TraceEnd,FColor::Blue,false,10.0f,0.0f,1.0f);
		if (HitResult.Num() >= 2)
		{
			// here got some issue....//
			UE_LOG(LogTemp, Log, TEXT("TWO %d"), HitResult.Num());
			if (HitResult[1].Distance < TargetDistance)
			{
				TargetDistance = HitResult[1].Distance;
				NearestVertice = i;
			}
		}
		else if(HitResult.Num() == 1)
		{
			DrawDebugPoint(GetWorld(), HitResult[0].Location, 20, FColor::Orange, false, 10);
			UE_LOG(LogTemp, Log, TEXT("one : %f"), HitResult[0].Distance);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("zero"));
		}
		
		
	}
	float CurDistance = (Vertices[NearestVertice] - CamLoc).Size();
	float ScaleFactor = 1.0f;
	if (TargetDistance < 9999)
	{
		ScaleFactor = TargetDistance / CurDistance;
	}
	else
	{
		ScaleFactor = 1;
	}
	//float ScaleFactor = TargetDistance / CurDistance;

	UE_LOG(LogTemp, Error, TEXT("ScaleFactor is %f"), TargetDistance);
	
	return ScaleFactor;
}



void URescaleComponent::Rescale(FHitResult HitResult)
{
	TArray<FVector> Vs = GetAllVertices(HitResult);
	float ScaleFactor = URescaleComponent::CastRayToAll(Vs);
	FVector CurScale = HitResult.GetActor()->GetActorScale3D();
	FVector CurLocation = HitResult.GetActor()->GetActorLocation();
	// FVector TargetScale = CurScale * ScaleFactor;
	// FVector TargetLocation = CurLocation * ScaleFactor;
	HitResult.GetActor()->SetActorScale3D(CurScale * ScaleFactor);
	HitResult.GetActor()->SetActorLocation(CurLocation * ScaleFactor);

}
// float scaleFactor = curDistance / targetDistance;
// targetScale = currentScale * scaleFactor;

