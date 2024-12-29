#if WITH_TESTS
#include <RetroLib/Ranges/Views/Filter.h>
#include <RetroLib/Ranges/Algorithm/To.h>
#include "RetroLib/Ranges/Views/ObjectView.h"
#include "RetroLib/Ranges/Compatibility/Array.h"
#include "Tests/TestHarnessAdapter.h"
#include "RetroLib/Ranges/Views/ClassView.h"

TEST_CASE_NAMED(FObjectViewTest, "RetroLib::Ranges::Views::ObjectView", "[RetroLib][Ranges]") {
	SECTION("Can iterate over a view of objects") {
		static_assert(std::ranges::input_range<retro::ranges::TObjectView<UClass>>);
		auto ActorClasses = retro::ranges::TObjectView<UClass>() |
			retro::ranges::views::filter([](const UClass* Class) { return Class->IsChildOf<AActor>(); }) |
				retro::ranges::to<TArray>();
		CHECK(ActorClasses.Num() > 0);
		CHECK(std::ranges::all_of(ActorClasses, [](const UClass* Class) { return Class->IsChildOf<AActor>(); }));
	}
}

TEST_CASE_NAMED(FClassViewTest, "RetroLib::Ranges::Views::ClassView", "[RetroLib][Ranges]") {
	SECTION("Can iterate over a view of classes") {
		static_assert(std::ranges::input_range<retro::ranges::TClassView<AActor>>);
		auto ActorClasses = retro::ranges::TClassView<AActor>() |
				retro::ranges::to<TArray>();
		CHECK(ActorClasses.Num() > 0);
		CHECK(std::ranges::all_of(ActorClasses, [](const UClass* Class) { return Class->IsChildOf<AActor>(); }));
	}
}

#endif