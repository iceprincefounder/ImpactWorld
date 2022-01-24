// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "ImpactFoliageComponent.generated.h"

USTRUCT(BlueprintType)
struct FImpactFoliageParams
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	float ImpactStrength = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	float ImpactSizeInPercent = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractParam)
	FVector2D ImpactSizeRange = FVector2D(200.0f, 2000.0f);
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = ImpactWorld, meta = (BlueprintSpawnableComponent, DisplayName = "ImpactFoliageComponent"))
class IMPACTWORLD_API UImpactFoliageComponent : public UActorComponent
{
    GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactFoliage)
	float StartTraceHeight = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactFoliage)
	float EndTraceHeight = -100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactFoliage)
	FImpactFoliageParams ImpactFoliageSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactFoliage)
	uint8 bDrawDebugMode : 1;
public:
	//~ Begin UActorComponent interface
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UninitializeComponent() override;
	//~ End UActorComponent interface

protected:
	void SyncInteractiveDrawingJobs(float Velocity, float DeltaTime);
	void DrawInteractiveDebugInfo();

private:
	uint8 bHitCollision : 1;
	uint8 bIsIntersected : 1;
	FVector HitCollisionLocation;
	FVector CurrLocation;
	FVector LastLocation;
};