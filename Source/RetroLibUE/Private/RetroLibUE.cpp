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
	static_assert(retro::ranges::UnrealAppendable<TArray<int>, int>);
	retro::ranges::AppendableContainerType<TArray<int>>::is_valid;
	TArray<int32> Array;
	retro::ranges::append_container(Array, 5);
	static_assert(retro::ranges::AppendableContainer<TArray<int32>, int32>);
	std::array<int, 3> ArrayType = { 1, 2, 3 };
	TSet<int32> SetType = { 1, 2, 3 };
	static_assert(retro::ranges::ReservableContainer<TArray<int32>>);
	auto Transformed2 = SetType |
		std::ranges::views::transform([](int32 i) { return i * 2; }) |
			retro::ranges::to<TArray>();

	static_assert(retro::optionals::OptionalType<TOptional<int32>>);
	TOptional<int32> OptionalValue;
	static_assert(std::same_as<int32&, retro::optionals::CommonReference<TOptional<int32>&>>);
	auto filtered = OptionalValue |
		retro::optionals::filter([](int32 i) { return i > 1;});
}

void FRetroLibUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRetroLibUEModule, RetroLibUE)