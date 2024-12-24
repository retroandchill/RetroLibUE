#if WITH_TESTS

#include "RetroLib/Functional/Delegates.h"
#include "RetroLib/Ranges/Algorithm/To.h"
#include "RetroLib/Ranges/Compatibility/UnrealContainers.h"
#include "RetroLib/Ranges/Views/Filter.h"
#include "Tests/TestHarnessAdapter.h"

#include <array>

namespace retro::testing::delegates {
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FDemoDelegate, FStringView, int32);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FDemoMulticastDelegate, FStringView, int32);
    DECLARE_DELEGATE_RetVal(FString, FGetObjectName);
    DECLARE_DELEGATE_OneParam(FAddToValue, int32)

        DECLARE_DELEGATE_OneParam(FAddToArray, TArray<int32> &);

    static_assert(retro::NativeDelegate<FDemoDelegate>);
    static_assert(retro::UnicastDelegate<FDemoDelegate>);
    static_assert(retro::UEDelegate<FDemoDelegate>);
    static_assert(retro::NativeDelegate<FDemoMulticastDelegate>);
    static_assert(retro::MulticastDelegate<FDemoMulticastDelegate>);
    static_assert(retro::UEDelegate<FDemoMulticastDelegate>);

    static bool IsLength(FStringView View, int32 Length) {
        return View.Len() == Length;
    }

    static void AddValue(TArray<int32> &Array, int32 Value) {
        Array.Add(Value);
    }

    class FDemoClass : public TSharedFromThis<FDemoClass> {
      public:
        explicit FDemoClass(int32 Value) : Value(Value) {
        }

        int32 GetValue() const {
            return Value;
        }

        void AddToValue(int32 Other) {
            Value += Other;
        }

      private:
        int32 Value;
    };

    class FUnsharedDemoClass {
      public:
        explicit FUnsharedDemoClass(int32 Value) : Value(Value) {
        }

        int32 GetValue() const {
            return Value;
        }

        void AddToValue(int32 Other) {
            Value += Other;
        }

      private:
        int32 Value;
    };
} // namespace retro::testing::delegates

TEST_CASE_NAMED(FDelegateBindingTest, "RetroLib::Functional::Delegates::Wrapping", "[RetroLib][Functional]") {
    using namespace retro::testing::delegates;

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
        std::array Strings = {FStringView(TEXT("1234567890")), FStringView(TEXT("123456789"))};

        auto ValidStrings = Strings | retro::ranges::views::filter(FDemoDelegate::CreateStatic(&IsLength), 10) |
                            retro::ranges::to<TArray>();

        REQUIRE(ValidStrings.Num() == 1);
        CHECK(ValidStrings[0] == TEXT("1234567890"));
    }
}

TEST_CASE_NAMED(FCreateDelegateTest, "RetroLib::Functional::Delegates::Creation", "[RetroLib][Functional]") {
    using namespace retro::testing::delegates;

    SECTION("Can bind free functions and lambdas") {
        static_assert(retro::CanBindStatic<FAddToArray, decltype(&AddValue), int32>);
        static_assert(retro::CanBindLambda<FAddToArray, decltype(&AddValue), int32>);
        TArray<int32> Array;
        auto Delegate1 = retro::CreateDelegate<FAddToArray>(&AddValue, 4);
        Delegate1.Execute(Array);
        REQUIRE(Array.Num() == 1);
        CHECK(Array[0] == 4);

        int Value = 12;
        auto Lambda = [Value](TArray<int32> &A) { A.Add(Value); };
        static_assert(!retro::CanBindStatic<FAddToArray, decltype(Lambda)>);
        static_assert(retro::CanBindLambda<FAddToArray, decltype(Lambda)>);
        auto Delegate2 = retro::CreateDelegate<FAddToArray>(Lambda);
        Delegate2.Execute(Array);
        REQUIRE(Array.Num() == 2);
        CHECK(Array[1] == 12);
    }

    SECTION("Can bind UObject members") {
        auto Object = NewObject<UObject>();
        static_assert(retro::CanBindUObject<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::CanBindWeakLambda<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::CanBindSP<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::CanBindSPLambda<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(retro::CanBindRaw<FGetObjectName, UObject *, FString (UObject::*)() const>);
        auto Delegate1 =
            retro::CreateDelegate<FGetObjectName>(Object, static_cast<FString (UObject::*)() const>(&UObject::GetName));
        CHECK(Delegate1.Execute() == Object->GetName());

        int Value = 12;
        auto Lambda = [&Value] { Value *= 2; };
        static_assert(retro::CanBindWeakLambda<FSimpleDelegate, UObject *, decltype(Lambda)>);
        static_assert(!retro::CanBindSPLambda<FSimpleDelegate, UObject *, decltype(Lambda)>);
        auto Delegate2 = retro::CreateDelegate<FSimpleDelegate>(Lambda);
        Delegate2.Execute();
        CHECK(Value == 24);
    }

    SECTION("Can bind to shared pointers") {
        auto SharedValue = MakeShared<FDemoClass>(12);
        static_assert(!retro::CanBindUObject<FGetObjectName, TSharedRef<FDemoClass> &, FString (UObject::*)() const>);
        static_assert(!retro::CanBindUObject<FGetObjectName, FDemoClass *, FString (UObject::*)() const>);
        static_assert(retro::CanBindSP<FAddToValue, TSharedRef<FDemoClass> &, decltype(&FDemoClass::AddToValue)>);
        static_assert(retro::CanBindSP<FAddToValue, FDemoClass *, decltype(&FDemoClass::AddToValue)>);

        auto Delegate1 = retro::CreateDelegate<FAddToValue>(SharedValue, &FDemoClass::AddToValue);
        Delegate1.Execute(10);
        CHECK(SharedValue->GetValue() == 22);

        auto Delegate2 = retro::CreateDelegate<FAddToValue>(&SharedValue.Get(), &FDemoClass::AddToValue);
        Delegate2.Execute(13);
        CHECK(SharedValue->GetValue() == 35);

        auto Lambda = [&RawValue = SharedValue.Get()](int32 Value) { RawValue.AddToValue(Value); };
        static_assert(retro::CanBindSPLambda<FAddToValue, TSharedRef<FDemoClass> &, decltype(Lambda)>);
        static_assert(retro::CanBindSPLambda<FAddToValue, FDemoClass *, decltype(Lambda)>);
        auto Delegate3 = retro::CreateDelegate<FAddToValue>(SharedValue, Lambda);
        Delegate3.Execute(15);
        CHECK(SharedValue->GetValue() == 50);
    }

    SECTION("Can bind to raw pointers") {
        FUnsharedDemoClass RawValue(12);

        static_assert(retro::CanBindRaw<FAddToValue, FUnsharedDemoClass *, decltype(&FUnsharedDemoClass::AddToValue)>);
        static_assert(
            !retro::CanBindUObject<FAddToValue, FUnsharedDemoClass *, decltype(&FUnsharedDemoClass::AddToValue)>);
        static_assert(!retro::CanBindSP<FAddToValue, FUnsharedDemoClass *, decltype(&FDemoClass::AddToValue)>);

        auto Delegate1 = retro::CreateDelegate<FAddToValue>(&RawValue, &FUnsharedDemoClass::AddToValue);
        Delegate1.Execute(10);
        CHECK(RawValue.GetValue() == 22);
    }
}

#endif