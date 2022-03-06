#include "TestComputeShader.h"

IMPLEMENT_GLOBAL_SHADER(FTestComputeShader, "/VulkanComputeTesting/Private/TestShader.usf",	"MainCS",	SF_Compute)