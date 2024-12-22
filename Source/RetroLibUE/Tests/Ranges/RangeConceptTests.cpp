#include "RetroLib/Ranges/Views/Elements.h"
#if WITH_TESTS

#include "RetroLib/Ranges/Compatibility/UnrealContainers.h"
#include "RetroLib/Ranges/Views/Transform.h"
#include "Tests/TestHarnessAdapter.h"
#include "RetroLib/Ranges/Views/JoinWith.h"
#include "RetroLib/Ranges/Algorithm/To.h"

#include <array>

TEST_CASE_NAMED(FRangePipeTest, "RetroLib::Ranges::Compatibility", "[RetroLib][Compatibility]") {
	SECTION("Can iterate over an instance of TArray") {
		static_assert(std::ranges::input_range<TArray<int32>>);
		TArray Container = {1, 2, 3, 4, 5};
		auto Transformed = Container |
			retro::ranges::views::transform([](int32 I) { return I * 2; });

		int32 Sum = 0;
		for (auto I : Transformed) {
			Sum += I;
		}
		CHECK(Sum == 30);
	}

	SECTION("Can iterate over an instance of TSet") {
		static_assert(std::ranges::input_range<TSet<int32>>);
		TSet Container = { 1, 2, 3, 4, 5 };
		auto Transformed = Container |
			retro::ranges::views::transform([](int32 I) { return I * 2; });

		int32 Sum = 0;
		for (auto I : Transformed) {
			Sum += I;
		}
		CHECK(Sum == 30);
	}

	SECTION("Can iterate over an instance of TMap") {
		static_assert(std::ranges::input_range<TMap<int32, int32>>);
		static_assert(retro::TupleLike<std::ranges::range_value_t<TMap<int32, int32>>>);
		TMap<int32, int32> Container = { { 1, 2 }, { 3, 4 }, { 5, 6 } };
		auto Transformed = Container |
			retro::ranges::views::values |
			retro::ranges::views::transform([](int32 I) { return I * 2; });

		int32 Sum = 0;
		for (auto I : Transformed) {
			Sum += I;
		}
		CHECK(Sum == 24);
	}

	SECTION("Can use this to join a string") {
		SECTION("Can splice a string together with a character") {
			static_assert(std::ranges::input_range<FStringView>);
			static_assert(std::ranges::sized_range<FString>);
			static_assert(retro::ranges::ReservableContainer<FString>);
			std::array Strings = {FStringView(TEXT("This")), FStringView(TEXT("is")), FStringView(TEXT("a")), FStringView(TEXT("test."))};
			auto Joined = Strings |
				retro::ranges::views::join_with(static_cast<TCHAR>(' ')) |
					retro::ranges::to<FString>();
			CHECK(Joined == TEXT("This is a test."));
		}
		
		SECTION("Can splice a string together with a string literal") {
			std::array Strings = {FStringView(TEXT("1")), FStringView(TEXT("2")), FStringView(TEXT("3")), FStringView(TEXT("4")) };
			auto Constraction = FStringView(TEXT(", "));
			auto Joined = Strings | retro::ranges::views::join_with(Constraction) | retro::ranges::to<FString>();
			CHECK(Joined == TEXT("1, 2, 3, 4"));
		}
	}
}

#endif // WITH_TESTS