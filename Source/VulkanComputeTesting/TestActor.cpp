// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor.h"
#include "TestComputeShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Launch/Resources/Version.h"

#define UE5 ENGINE_MAJOR_VERSION > 4

void FTestShaderTicker::Tick(float DeltaTime)
{
	if (!BufferReadback.IsReady())
	{
		return;
	}

	const uint32 BufferSize = NumElements * ElementSize;
	
	TArray<int32> ReadbackBuffer;
	ReadbackBuffer.SetNumUninitialized(NumElements);
	for (int32& Elem : ReadbackBuffer)
	{
		Elem = 2;
	}
	
	const int32* SrcPtr = static_cast<int32*>(BufferReadback.Lock(BufferSize));
	
	FMemory::Memcpy(ReadbackBuffer.GetData(), SrcPtr, BufferSize);

	BufferReadback.Unlock();
	
	bCompleted = true;
}

// Sets default values
ATestActor::ATestActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();

	ENQUEUE_RENDER_COMMAND(RunRasteriseLayerOneComputeShader)([this](FRHICommandListImmediate& RHICmdList)
	{
		constexpr int32 NumElements = 1024;
		constexpr uint32 ElementSize = sizeof(int32);
		
		TestShaderTicker = MakeUnique<FTestShaderTicker>(NumElements, ElementSize);
		Run_RenderThread(RHICmdList, *TestShaderTicker);
	});
}

BEGIN_SHADER_PARAMETER_STRUCT(FReadbackParams, )
	RDG_BUFFER_ACCESS(SrcBuffer, ERHIAccess::CopySrc)
END_SHADER_PARAMETER_STRUCT()

void ATestActor::Run_RenderThread(FRHICommandListImmediate& RHICmdList, FTestShaderTicker& Ticker)
{
	FRDGBuilder GraphBuilder(RHICmdList);

	const uint32 TestBufferSize = Ticker.ElementSize * Ticker.NumElements;
	
	// Create empty structured buffer
	FRDGBufferDesc Desc = FRDGBufferDesc::CreateBufferDesc(Ticker.ElementSize, Ticker.NumElements);
	Desc.Usage |= BUF_SourceCopy;
	const FRDGBufferRef TestBuffer = GraphBuilder.CreateBuffer(Desc, TEXT("TestBuffer"));
	const FRDGBufferUAVRef TestUAV = GraphBuilder.CreateUAV(TestBuffer, PF_R32_SINT);
	
	FTestComputeShader::FParameters* TestShaderParameters = GraphBuilder.AllocParameters<FTestComputeShader::FParameters>();
	TestShaderParameters->TestUAV = TestUAV;
	
	const TShaderMapRef<FTestComputeShader> TestShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	const FIntVector GroupCount = FIntVector(Ticker.NumElements / 64, 1, 1);

	FComputeShaderUtils::AddPass(GraphBuilder,
		RDG_EVENT_NAME("Test Shader Dispatch"),
		TestShader,
		TestShaderParameters,
		GroupCount);
	
	//FComputeShaderUtils::Dispatch(RHICmdList, TestShader, TestShaderParameters, GroupCount);
	FReadbackParams* ReadbackParams = GraphBuilder.AllocParameters<FReadbackParams>();
	ReadbackParams->SrcBuffer = TestBuffer;
	
	GraphBuilder.AddPass(
		RDG_EVENT_NAME("Test Shader Readback"),
		ReadbackParams,
		ERDGPassFlags::Copy | ERDGPassFlags::NeverCull,
		[&BufferReadback = Ticker.BufferReadback, &ReadbackParams, TestBufferSize](FRHICommandList& RHICmdList)
	{
		BufferReadback.EnqueueCopy(RHICmdList, ReadbackParams->SrcBuffer->GetRHIVertexBuffer(), TestBufferSize);
		ReadbackParams->SrcBuffer->MarkResourceAsUsed();
	});
	
	GraphBuilder.RemoveUnusedBufferWarning(TestBuffer);
	
	/*FStagingBufferRHIRef DestinationStagingBuffer = RHICreateStagingBuffer();
	FGPUFenceRHIRef GPUFence = RHICreateGPUFence(TEXT("TestBufferReadback"));
	GPUFence->Clear();
	RHICmdList.CopyToStagingBuffer(TestBuffer, DestinationStagingBuffer, 0, TestBufferSize);
	RHICmdList.WriteGPUFence(GPUFence);
	RHICmdList.SubmitCommandsAndFlushGPU();
	RHICmdList.BlockUntilGPUIdle();*/

	GraphBuilder.Execute();
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TestShaderTicker && TestShaderTicker->bCompleted)
	{
		ENQUEUE_RENDER_COMMAND(RunRasteriseLayerOneComputeShader)([this](FRHICommandListImmediate& RHICmdList)
		{
			TestShaderTicker = nullptr;
		});
	}
}

