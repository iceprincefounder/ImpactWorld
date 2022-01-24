// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#include "ImpactWaterComponent.h"
#include "ImpactWorldSubSystem.h"
#include "Engine/CollisionProfile.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<bool> CVarImpactWaterDrawDebug(TEXT("water.agent.drawdebug"), false, TEXT("Enable Draw Debug"));

UImpactWaterComponent::UImpactWaterComponent(const FObjectInitializer& ObjectInitializer) :
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
	bIsAboveWater = false;
	bIsUnderWater = false;
	bIsIntersected = false;
	HitCollisionLocation = FVector::ZeroVector;
	IntersectedDepth = 0.0f;
	ActiveIdleLifeTime = 5.0f;

	// Default interact params values.
	CurrentInteractiveMode = EWaterInteractState::Idling;
	IdlingInWaterSetting.FoamStrength = 0.25f;
	IdlingInWaterSetting.FoamSizeInPercent = 1.0f;
	IdlingInWaterSetting.FoamSizeRange = FVector2D(500.0f, 3000.0f);
	RunningInWaterSetting.FoamStrength = 0.8f;
	RunningInWaterSetting.FoamSizeInPercent = 1.0f;
	RunningInWaterSetting.FoamSizeRange = FVector2D(200.0f, 2000.0f);
	SwimingInWaterSetting.PivotForward = 100.0f;
	SwimingInWaterSetting.FoamStrength = 0.8f;
	SwimingInWaterSetting.FoamSizeInPercent = 1.0f;
	SwimingInWaterSetting.FoamSizeRange = FVector2D(100.0f, 2000.0f);
}

void UImpactWaterComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UImpactWaterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	CurrLocation = GetOwner()->GetActorLocation();
	FVector Velocity = (CurrLocation - LastLocation) / DeltaTime;
	LastLocation = CurrLocation;

	// Do Trace Collision Task.
	FVector StartTracePoint = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, StartTraceHeight);
	FVector EndTracePoint = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, EndTraceHeight);
	// Shot a Z-Axis ray in the scene.
	FHitResult HitResult;
	// Custom trace channel, ECC_GameTraceChannel13 == AcInterWater
	bool bHitWorld = GetWorld()->LineTraceSingleByChannel(HitResult, StartTracePoint, EndTracePoint, ECollisionChannel::ECC_Visibility);
	bHitCollision = false;
	if (bHitWorld)
	{
		bHitCollision = true;
		FVector HitLocation = HitResult.Location;
		FName ProfileName = HitResult.Component->GetCollisionProfileName();
		if (ProfileName.IsEqual(UCollisionProfile::DefaultProjectile_ProfileName) || ProfileName.IsEqual(UCollisionProfile::NoCollision_ProfileName) || ProfileName.IsEqual(FName("Custom")))
		{
			FVector StartPoint = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, StartTraceHeight);
			FVector EndPoint = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, EndTraceHeight);
			HitCollisionLocation = HitLocation;
			if (HitCollisionLocation.Z > EndPoint.Z && HitCollisionLocation.Z < StartPoint.Z)
			{
				bIsAboveWater = false;
				bIsUnderWater = false;
				bIsIntersected = true;
				IntersectedDepth = HitCollisionLocation.Z - EndPoint.Z;
			}
			else if(HitCollisionLocation.Z > EndPoint.Z && HitCollisionLocation.Z > StartPoint.Z)
			{
				bIsAboveWater = false;
				bIsUnderWater = true;
				bIsIntersected = false;
				IntersectedDepth = 0.0f;
			}
			else if (HitCollisionLocation.Z < EndPoint.Z && HitCollisionLocation.Z < StartPoint.Z)
			{
				bIsAboveWater = true;
				bIsUnderWater = false;
				bIsIntersected = false;
				IntersectedDepth = 0.0f;
			}
			else
			{
				bIsAboveWater = false;
				bIsUnderWater = false;
				bIsIntersected = false;
				IntersectedDepth = 0.0f;
			}
		}
	}
	else
	{
		bIsAboveWater = false;
		bIsUnderWater = false;
		bIsIntersected = false;
		IntersectedDepth = 0.0f;
	}
	SyncInteractiveDrawingJobs(Velocity.Size(), DeltaTime);

	// Do Actor Buoyancy Task.
	// To Do...

	DrawInteractiveDebugInfo(); // Draw Debug Infos.
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UImpactWaterComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

void UImpactWaterComponent::SyncInteractiveDrawingJobs(float Velocity, float DeltaTime)
{
	auto AddForceSplat = [this](UImpactWorldSubSystem* SubSystemPtr, const FWaterInteractParams& InteractParams, const FVector& ForwardVector)
	{
		FVector AddForceLocation = this->HitCollisionLocation + ForwardVector * InteractParams.PivotForward;
		SubSystemPtr->AddForceWaterSplat(AddForceLocation, InteractParams);
	};

	UImpactWorldSubSystem* SubSystem = GetWorld()->GetSubsystem<UImpactWorldSubSystem>();
	if (SubSystem && bIsIntersected)
	{
		FVector ForwardVector = GetOwner()->GetActorForwardVector();
		if (Velocity > 0.5f) // Running or Swimming
		{
			if (CurrentInteractiveMode == EWaterInteractState::Swimming)
			{
				FVector AddForceLocation = this->HitCollisionLocation + ForwardVector * SwimingInWaterSetting.PivotForward;
				SubSystem->OnAddForceWaterSplatEventChanged().Broadcast(AddForceLocation, SwimingInWaterSetting);
			}
			else
			{
				FVector AddForceLocation = this->HitCollisionLocation + ForwardVector * RunningInWaterSetting.PivotForward;
				SubSystem->OnAddForceWaterSplatEventChanged().Broadcast(AddForceLocation, RunningInWaterSetting);
			}
		}
		else // Idling
		{
			if (ActiveIdleLifeTime >= IdlingInWaterSetting.IdleWaveLiftTime)
			{
				// Idling interact with water.
				FVector AddForceLocation = this->HitCollisionLocation + ForwardVector * IdlingInWaterSetting.PivotForward;
				SubSystem->OnAddForceWaterSplatEventChanged().Broadcast(AddForceLocation, IdlingInWaterSetting);
				ActiveIdleLifeTime = 0.0f;
			}
			else
			{
				float Noise = FMath::RandRange(0.0f, 2.0f);
				ActiveIdleLifeTime += DeltaTime * Noise;
			}
		}
	}
}

void UImpactWaterComponent::DrawInteractiveDebugInfo()
{
	if (CVarImpactWaterDrawDebug.GetValueOnAnyThread() || bDrawDebugMode)
	{
		FVector CharPos = GetOwner()->GetActorLocation();
#if ENABLE_DRAW_DEBUG
		::DrawDebugLine(GetWorld(), CharPos + FVector(0.0f, 0.0f, HALF_WORLD_MAX), CharPos + FVector(0.0f, 0.0f, -HALF_WORLD_MAX), bHitCollision ? FColor::Red : FColor::White);
		::DrawDebugSphere(GetWorld(), CharPos + FVector(0.0f, 0.0f, StartTraceHeight), 20.0f, 4, bIsUnderWater ? FColor::Red : FColor::White);
		::DrawDebugSphere(GetWorld(), CharPos + FVector(0.0f, 0.0f, EndTraceHeight), 20.0f, 4, bIsAboveWater ? FColor::Red : FColor::White);
		if (bIsIntersected)
		{
			::DrawDebugSphere(GetWorld(), HitCollisionLocation, 50.0f, 4, FColor::Red);
		}
#endif
	}
}
