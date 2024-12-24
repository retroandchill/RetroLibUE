#if WITH_TESTS

#include "Tests/TestHarnessAdapter.h"
#include "RetroLib/Functional/Delegates.h"
#include "RetroLib/Ranges/Compatibility/UnrealContainers.h"
#include "RetroLib/Ranges/Algorithm/To.h"
#include "RetroLib/Ranges/Views/Filter.h"

#include <array>

DECLARE_DELEGATE_RetVal_TwoParams(bool, FDemoDelegate, FStringView, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FDemoMulticastDelegate, FStringView, int32);

static_assert(retro::NativeDelegate<FDemoDelegate>);
static_assert(retro::UnicastDelegate<FDemoDelegate>);
static_assert(retro::UEDelegate<FDemoDelegate>);
static_assert(retro::NativeDelegate<FDemoMulticastDelegate>);
static_assert(retro::MulticastDelegate<FDemoMulticastDelegate>);
static_assert(retro::UEDelegate<FDemoMulticastDelegate>);

static bool IsLength(FStringView View, int32 Length) {
	return View.Len() == Length;
}

TEST_CASE_NAMED(FDelegateBindingTest, "RetroLib::Functional::Delegates", "[RetroLib][Functional]") {
	SECTION("Should be able to bind a regular delegate") {
		FDemoDelegate Delegate;
		auto Binding = retro::create_binding(FDemoDelegate::CreateStatic(&IsLength), 10);
		CHECK(Binding(TEXT("1234567890")));
		CHECK_FALSE(Binding(TEXT("123456789")));
	}

	SECTION("Should be able to bind a multicast delegate") {
		FDemoMulticastDelegate Delegate;
		int32 Findings = 0;
		Delegate.Add(FDemoMulticastDelegate::FDelegate::CreateLambda([&Findings](FStringView View, int32 Length) {
			if (View.Len() == Length) {
				Findings++;
			}
		}));
		
		auto Binding = retro::create_binding(Delegate, 10);
		Binding(TEXT("1234567890"));
		Binding(TEXT("123456789"));
		CHECK(Findings == 1);
	}

	SECTION("Can binding a delegate to a range pipe") {
		std::array Strings = { FStringView(TEXT("1234567890")), FStringView(TEXT("123456789")) };

		auto ValidStrings = Strings |
			retro::ranges::views::filter(FDemoDelegate::CreateStatic(&IsLength), 10) |
				retro::ranges::to<TArray>();

		REQUIRE(ValidStrings.Num() == 1);
		CHECK(ValidStrings[0] == TEXT("1234567890"));
	}
}

#endif