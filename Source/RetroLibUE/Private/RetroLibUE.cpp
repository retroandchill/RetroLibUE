// Copyright Epic Games, Inc. All Rights Reserved.

#include "RetroLibUE.h"
#include "RetroLib/Ranges/Algorithm/To.h"
#include "RetroLib/Optionals/Compatiblity/UnrealOptional.h"
#include "RetroLib/Optionals/Filter.h"
#include "RetroLib/Ranges/Compatibility/UnrealContainers.h"

#include <array>


#define LOCTEXT_NAMESPACE "FRetroLibUEModule"


void FRetroLibUEModule::StartupModule()
{
}

void FRetroLibUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRetroLibUEModule, RetroLibUE)