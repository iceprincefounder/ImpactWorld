// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#include "ImpactFoliageComponent.h"
#include "ImpactWorldSubSystem.h"
#include "Engine/CollisionProfile.h"
#include "DrawDebugHelpers.h"

UImpactFoliageComponent::UImpactFoliageComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	SetComponentTickEnabled(true);
	bTickInEditor = true;
	bAutoActivate = true;
	bWantsInitializeComponent = true;

	bHitCollision = false;
	bIsIntersected = false;
	HitCollisionLocation = FVector::ZeroVector;

	ImpactFoliageSetting.ImpactStrength = 0.8f;
	ImpactFoliageSetting.ImpactSizeInPercent = 1.0f;
	ImpactFoliageSetting.ImpactSizeRange = FVector2D(200.0f, 2000.0f);
}

void UImpactFoliageComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UImpactFoliageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	CurrLocation = GetOwner()->GetActorLocation();
	FVector Velocity = (CurrLocation - LastLocation) / DeltaTime;
	LastLocation = CurrLocation;

	// Do Trace Collision Task.
	FVector StartTracePoint = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, StartTraceHeight);
	FVector EndTracePoint = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, EndTraceHeight);
	// Shot a Z-Axis ray in the scene.
	TArray<FHitResult> HitResults;
	// Custom trace channel, ECC_GameTraceChannel13 == AcInterWater
	bool bHitWorld = GetWorld()->LineTraceMultiByChannel(HitResults, StartTracePoint, EndTracePoint, ECollisionChannel::ECC_Visibility);
	bHitCollision = false;
	if (bHitWorld)
	{
		for (FHitResult& HitResult : HitResults)
		{			
			if (HitResult.Component->GetOwner() != GetOwner())
			{
				bHitCollision = true;
				FVector HitLocation = HitResult.Location;
				if (HitLocation.Z <= StartTracePoint.Z && HitLocation.Z >= EndTracePoint.Z)
				{
					bIsIntersected = true;
					HitCollisionLocation = HitLocation;
				}
			}			
		}
	}
	else
	{
		bIsIntersected = false;
	}
	SyncInteractiveDrawingJobs(Velocity.Size(), DeltaTime);

	DrawInteractiveDebugInfo(); // Draw Debug Infos.
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UImpactFoliageComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

void UImpactFoliageComponent::SyncInteractiveDrawingJobs(float Velocity, float DeltaTime)
{
	UImpactWorldSubSystem* SubSystem = GetWorld()->GetSubsystem<UImpactWorldSubSystem>();
	if (SubSystem && bIsIntersected)
	{
		SubSystem->OnAddForceFoliageSplatEventChanged().Broadcast(this->HitCollisionLocation, ImpactFoliageSetting);
	}
}

void UImpactFoliageComponent::DrawInteractiveDebugInfo()
{
	if (bDrawDebugMode)
	{
		FVector CharPos = GetOwner()->GetActorLocation();
#if ENABLE_DRAW_DEBUG
		::DrawDebugLine(GetWorld(), CharPos + FVector(0.0f, 0.0f, StartTraceHeight), CharPos + FVector(0.0f, 0.0f, EndTraceHeight), bHitCollision ? FColor::Red : FColor::White);
		::DrawDebugSphere(GetWorld(), CharPos + FVector(0.0f, 0.0f, StartTraceHeight), 20.0f, 4, FColor::White);
		::DrawDebugSphere(GetWorld(), CharPos + FVector(0.0f, 0.0f, EndTraceHeight), 20.0f, 4, FColor::White);
		if (bIsIntersected)
		{
			::DrawDebugSphere(GetWorld(), HitCollisionLocation, 50.0f, 4, FColor::Red);
		}
#endif
	}
}