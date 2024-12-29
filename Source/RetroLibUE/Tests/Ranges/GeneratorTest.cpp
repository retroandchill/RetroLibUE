#if WITH_TESTS && RETROLIB_WITH_COROUTINES

#include "Tests/TestHarnessAdapter.h"
#include "RetroLib/Ranges/Compatibility/Array.h"
#include "RetroLib/Ranges/Algorithm/To.h"
#include "RetroLib/Ranges/Views/Generator.h"
#include "RetroLib/Ranges/Views/NameAliases.h"

namespace Retro::Testing::Generators {
	Generator<int32> GenerateInts(int32 Start) {
		while (true) {
			co_yield Start;
			Start++;
		}
	}
}

TEST_CASE_NAMED(FGeneratorTest, "RetroLib::Ranges::Views::Generator", "[RetroLib][Ranges]") {
	using namespace Retro::Testing::Generators;

	auto Sequence = GenerateInts(1) |
			Retro::Ranges::Views::Take(10) |
			Retro::Ranges::To<TArray>();
	REQUIRE(Sequence.Num() == 10);
	CHECK(Sequence == TArray({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
}

#endif
