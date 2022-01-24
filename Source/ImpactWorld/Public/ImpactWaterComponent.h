// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "ImpactWaterComponent.generated.h"

UENUM(BlueprintType)
enum class EWaterInteractState : uint8
{
	None = 0,	// No Interaction
	Idling,		// Character Idling
	Running,	// Character Running
	Swimming,	// Character Swimming
	Anchoring,	// Ships Anchoring
	Sailing		// Ships Sailing
};

USTRUCT(BlueprintType)
struct FWaterInteractParams
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	float PivotForward = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	float RippleStrength = 0.5f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	float RippleSizeInPercent = 0.25f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	FVector2D RippleSizeRange = FVector2D(1000.0f, 3000.0f);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	float FoamStrength = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	float FoamSizeInPercent = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	FVector2D FoamSizeRange = FVector2D(200.0f, 2000.0f);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	float IdleWaveLiftTime = 2.5f;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = ImpactWorld, meta = (BlueprintSpawnableComponent, DisplayName = "ImpactWaterComponent"))
class IMPACTWORLD_API UImpactWaterComponent : public UActorComponent
{
    GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactWater)
	float StartTraceHeight = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactWater)
	float EndTraceHeight = -75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactWater)
	EWaterInteractState CurrentInteractiveMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactWater)
	FWaterInteractParams IdlingInWaterSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactWater)
	FWaterInteractParams RunningInWaterSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactWater)
	FWaterInteractParams SwimingInWaterSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactWater)
	uint8 bDrawDebugMode : 1;
public:
	//~ Begin UActorComponent interface
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UninitializeComponent() override;
	//~ End UActorComponent interface

public:
	UFUNCTION(BlueprintCallable, Category = ImpactWater)
	bool IsAboveWater() const { return bIsAboveWater; };

	UFUNCTION(BlueprintCallable, Category = ImpactWater)
	bool IsUnderWater() const { return bIsUnderWater; };

	UFUNCTION(BlueprintCallable, Category = ImpactWater)
	bool IsIntersectedWithWater() const { return bIsIntersected; };

	UFUNCTION(BlueprintCallable, Category = ImpactWater)
	FVector GetHitCollisionLocation() const { return HitCollisionLocation; };

	UFUNCTION(BlueprintCallable, Category = ImpactWater)
	float GetIntersectedDepth() const { return IntersectedDepth; };

	UFUNCTION(BlueprintCallable, Category = ImpactWater)
	void SetInteractiveMode(EWaterInteractState State) { CurrentInteractiveMode = State; };

protected:
	void SyncInteractiveDrawingJobs(float Velocity, float DeltaTime);
	void DrawInteractiveDebugInfo();

private:
	uint8 bHitCollision : 1;
	uint8 bIsAboveWater : 1;
	uint8 bIsUnderWater : 1;
	uint8 bIsIntersected : 1;
	FVector HitCollisionLocation;
	float IntersectedDepth;
	float ActiveIdleLifeTime;
	FVector CurrLocation;
	FVector LastLocation;
};