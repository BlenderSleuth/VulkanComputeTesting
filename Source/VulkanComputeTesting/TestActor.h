// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestActor.generated.h"

class VULKANCOMPUTETESTING_API FTestShaderTicker final : public FTickableObjectRenderThread
{
public:
	FTestShaderTicker(const int32 NumElements, const uint32 ElementSize):
		NumElements(NumElements),
		ElementSize(ElementSize),
		BufferReadback(TEXT("Test Buffer Readback"))
	{ }
	
	const int32 NumElements;
	const uint32 ElementSize;
	FRHIGPUBufferReadback BufferReadback;
	FThreadSafeBool bCompleted = false;
	
	virtual void Tick(float DeltaTime) override;;

	/** return the stat id to use for this tickable **/
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTestShaderTicker, STATGROUP_Tickables);
	}
	virtual bool IsTickable() const override
	{
		return !bCompleted;
	}
	virtual bool NeedsRenderingResumedForRenderingThreadTick() const final override
	{
		return false;
	}
};

UCLASS()
class VULKANCOMPUTETESTING_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

	// Only accessible from the render thread
	TUniquePtr<FTestShaderTicker> TestShaderTicker;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	static void Run_RenderThread(FRHICommandListImmediate& RHICmdList, FTestShaderTicker& Ticker);
		
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
