﻿#if WITH_TESTS

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
    
    DECLARE_MULTICAST_DELEGATE_OneParam(FMultiGetObjectName, FString&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FMultiAddToValue, int32)
    DECLARE_MULTICAST_DELEGATE_OneParam(FMultiAddToArray, TArray<int32> &);

    static_assert(retro::delegates::NativeDelegate<FDemoDelegate>);
    static_assert(retro::delegates::UnicastDelegate<FDemoDelegate>);
    static_assert(retro::delegates::UEDelegate<FDemoDelegate>);
    static_assert(retro::delegates::NativeDelegate<FDemoMulticastDelegate>);
    static_assert(retro::delegates::MulticastDelegate<FDemoMulticastDelegate>);
    static_assert(retro::delegates::UEDelegate<FDemoMulticastDelegate>);

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
        static_assert(retro::delegates::CanBindStatic<FAddToArray, decltype(&AddValue), int32>);
        static_assert(retro::delegates::CanBindLambda<FAddToArray, decltype(&AddValue), int32>);
        TArray<int32> Array;
        auto Delegate1 = retro::delegates::Create<FAddToArray>(&AddValue, 4);
        Delegate1.Execute(Array);
        REQUIRE(Array.Num() == 1);
        CHECK(Array[0] == 4);

        int Value = 12;
        auto Lambda = [Value](TArray<int32> &A) { A.Add(Value); };
        static_assert(!retro::delegates::CanBindStatic<FAddToArray, decltype(Lambda)>);
        static_assert(retro::delegates::CanBindLambda<FAddToArray, decltype(Lambda)>);
        auto Delegate2 = retro::delegates::Create<FAddToArray>(Lambda);
        Delegate2.Execute(Array);
        REQUIRE(Array.Num() == 2);
        CHECK(Array[1] == 12);
    }

    SECTION("Can bind UObject members") {
        auto Object = NewObject<UDataTable>();
        static_assert(retro::delegates::CanBindUObject<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::delegates::CanBindWeakLambda<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::delegates::CanBindSP<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::delegates::CanBindSPLambda<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(retro::delegates::CanBindRaw<FGetObjectName, UObject *, FString (UObject::*)() const>);
        auto Delegate1 =
            retro::delegates::Create<FGetObjectName>(Object, static_cast<FString (UObject::*)() const>(&UObject::GetName));
        CHECK(Delegate1.Execute() == Object->GetName());

        int Value = 12;
        auto Lambda = [&Value] { Value *= 2; };
        static_assert(retro::delegates::CanBindWeakLambda<FSimpleDelegate, UObject *, decltype(Lambda)>);
        static_assert(!retro::delegates::CanBindSPLambda<FSimpleDelegate, UObject *, decltype(Lambda)>);
        auto Delegate2 = retro::delegates::Create<FSimpleDelegate>(Lambda);
        Delegate2.Execute();
        CHECK(Value == 24);
    }

    SECTION("Can bind to shared pointers") {
        auto SharedValue = MakeShared<FDemoClass>(12);
        static_assert(!retro::delegates::CanBindUObject<FGetObjectName, TSharedRef<FDemoClass> &, FString (UObject::*)() const>);
        static_assert(!retro::delegates::CanBindUObject<FGetObjectName, FDemoClass *, FString (UObject::*)() const>);
        static_assert(retro::delegates::CanBindSP<FAddToValue, TSharedRef<FDemoClass> &, decltype(&FDemoClass::AddToValue)>);
        static_assert(retro::delegates::CanBindSP<FAddToValue, FDemoClass *, decltype(&FDemoClass::AddToValue)>);

        auto Delegate1 = retro::delegates::Create<FAddToValue>(SharedValue, &FDemoClass::AddToValue);
        Delegate1.Execute(10);
        CHECK(SharedValue->GetValue() == 22);

        auto Delegate2 = retro::delegates::Create<FAddToValue>(&SharedValue.Get(), &FDemoClass::AddToValue);
        Delegate2.Execute(13);
        CHECK(SharedValue->GetValue() == 35);

        auto Lambda = [&RawValue = SharedValue.Get()](int32 Value) { RawValue.AddToValue(Value); };
        static_assert(retro::delegates::CanBindSPLambda<FAddToValue, TSharedRef<FDemoClass> &, decltype(Lambda)>);
        static_assert(retro::delegates::CanBindSPLambda<FAddToValue, FDemoClass *, decltype(Lambda)>);
        auto Delegate3 = retro::delegates::Create<FAddToValue>(SharedValue, Lambda);
        Delegate3.Execute(15);
        CHECK(SharedValue->GetValue() == 50);
    }

    SECTION("Can bind to raw pointers") {
        FUnsharedDemoClass RawValue(12);

        static_assert(retro::delegates::CanBindRaw<FAddToValue, FUnsharedDemoClass *, decltype(&FUnsharedDemoClass::AddToValue)>);
        static_assert(
            !retro::delegates::CanBindUObject<FAddToValue, FUnsharedDemoClass *, decltype(&FUnsharedDemoClass::AddToValue)>);
        static_assert(!retro::delegates::CanBindSP<FAddToValue, FUnsharedDemoClass *, decltype(&FDemoClass::AddToValue)>);

        auto Delegate1 = retro::delegates::Create<FAddToValue>(&RawValue, &FUnsharedDemoClass::AddToValue);
        Delegate1.Execute(10);
        CHECK(RawValue.GetValue() == 22);
    }
}

TEST_CASE_NAMED(FBindDelegateTest, "RetroLib::Functional::Delegates::Binding", "[RetroLib][Functional]") {
    using namespace retro::testing::delegates;

    SECTION("Can bind free functions and lambdas") {
        static_assert(retro::delegates::CanBindStatic<FAddToArray, decltype(&AddValue), int32>);
        static_assert(retro::delegates::CanBindLambda<FAddToArray, decltype(&AddValue), int32>);
        TArray<int32> Array;
        FAddToArray Delegate1;
        retro::delegates::Bind(Delegate1, &AddValue, 4);
        Delegate1.Execute(Array);
        REQUIRE(Array.Num() == 1);
        CHECK(Array[0] == 4);

        int Value = 12;
        auto Lambda = [Value](TArray<int32> &A) { A.Add(Value); };
        static_assert(!retro::delegates::CanBindStatic<FAddToArray, decltype(Lambda)>);
        static_assert(retro::delegates::CanBindLambda<FAddToArray, decltype(Lambda)>);
        FAddToArray Delegate2;
        Delegate2 | retro::delegates::Bind(Lambda);
        Delegate2.Execute(Array);
        REQUIRE(Array.Num() == 2);
        CHECK(Array[1] == 12);
    }

    SECTION("Can bind UObject members") {
        auto Object = NewObject<UDataTable>();
        static_assert(retro::delegates::CanBindUObject<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::delegates::CanBindWeakLambda<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::delegates::CanBindSP<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(!retro::delegates::CanBindSPLambda<FGetObjectName, UObject *, FString (UObject::*)() const>);
        static_assert(retro::delegates::CanBindRaw<FGetObjectName, UObject *, FString (UObject::*)() const>);
        FGetObjectName Delegate1;
        retro::delegates::Bind(Delegate1, Object, static_cast<FString (UObject::*)() const>(&UObject::GetName));
        CHECK(Delegate1.Execute() == Object->GetName());

        int Value = 12;
        auto Lambda = [&Value] { Value *= 2; };
        static_assert(retro::delegates::CanBindWeakLambda<FSimpleDelegate, UObject *, decltype(Lambda)>);
        static_assert(!retro::delegates::CanBindSPLambda<FSimpleDelegate, UObject *, decltype(Lambda)>);
        FSimpleDelegate Delegate2;
        retro::delegates::Bind(Delegate2, Lambda);
        Delegate2.Execute();
        CHECK(Value == 24);
    }

    SECTION("Can bind to shared pointers") {
        auto SharedValue = MakeShared<FDemoClass>(12);
        static_assert(!retro::delegates::CanBindUObject<FGetObjectName, TSharedRef<FDemoClass> &, FString (UObject::*)() const>);
        static_assert(!retro::delegates::CanBindUObject<FGetObjectName, FDemoClass *, FString (UObject::*)() const>);
        static_assert(retro::delegates::CanBindSP<FAddToValue, TSharedRef<FDemoClass> &, decltype(&FDemoClass::AddToValue)>);
        static_assert(retro::delegates::CanBindSP<FAddToValue, FDemoClass *, decltype(&FDemoClass::AddToValue)>);

        FAddToValue Delegate1;
        retro::delegates::Bind(Delegate1, SharedValue, &FDemoClass::AddToValue);
        Delegate1.Execute(10);
        CHECK(SharedValue->GetValue() == 22);

        FAddToValue Delegate2;
        retro::delegates::Bind(Delegate2, &SharedValue.Get(), &FDemoClass::AddToValue);
        Delegate2.Execute(13);
        CHECK(SharedValue->GetValue() == 35);

        auto Lambda = [&RawValue = SharedValue.Get()](int32 Value) { RawValue.AddToValue(Value); };
        static_assert(retro::delegates::CanBindSPLambda<FAddToValue, TSharedRef<FDemoClass> &, decltype(Lambda)>);
        static_assert(retro::delegates::CanBindSPLambda<FAddToValue, FDemoClass *, decltype(Lambda)>);
        FAddToValue Delegate3;
        retro::delegates::Bind(Delegate3, SharedValue, Lambda);
        Delegate3.Execute(15);
        CHECK(SharedValue->GetValue() == 50);
    }

    SECTION("Can bind to raw pointers") {
        FUnsharedDemoClass RawValue(12);

        static_assert(retro::delegates::CanBindRaw<FAddToValue, FUnsharedDemoClass *, decltype(&FUnsharedDemoClass::AddToValue)>);
        static_assert(
            !retro::delegates::CanBindUObject<FAddToValue, FUnsharedDemoClass *, decltype(&FUnsharedDemoClass::AddToValue)>);
        static_assert(!retro::delegates::CanBindSP<FAddToValue, FUnsharedDemoClass *, decltype(&FDemoClass::AddToValue)>);

        FAddToValue Delegate1;
        retro::delegates::Bind(Delegate1, &RawValue, &FUnsharedDemoClass::AddToValue);
        Delegate1.Execute(10);
        CHECK(RawValue.GetValue() == 22);
    }
}

TEST_CASE_NAMED(FAddDelegateTest, "RetroLib::Functional::Delegates::Adding", "[RetroLib][Functional]") {
    using namespace retro::testing::delegates;

    SECTION("Can bind free functions and lambdas") {
        static_assert(retro::delegates::CanAddStatic<FMultiAddToArray, decltype(&AddValue), int32>);
        static_assert(retro::delegates::CanAddLambda<FMultiAddToArray, decltype(&AddValue), int32>);
        TArray<int32> Array;
        FMultiAddToArray Delegate1;
        retro::delegates::Add(Delegate1, &AddValue, 4);
        Delegate1.Broadcast(Array);
        REQUIRE(Array.Num() == 1);
        CHECK(Array[0] == 4);

        int Value = 12;
        auto Lambda = [Value](TArray<int32> &A) { A.Add(Value); };
        static_assert(!retro::delegates::CanAddStatic<FMultiAddToArray, decltype(Lambda)>);
        static_assert(retro::delegates::CanAddLambda<FMultiAddToArray, decltype(Lambda)>);
        FMultiAddToArray Delegate2;
        Delegate2 | retro::delegates::Add(Lambda);
        Delegate2.Broadcast(Array);
        REQUIRE(Array.Num() == 2);
        CHECK(Array[1] == 12);
    }

    SECTION("Can bind UObject members") {
        auto Object = NewObject<UDataTable>();
        static_assert(retro::delegates::CanAddUObject<FMultiGetObjectName, UObject *, void (UObject::*)(FString&) const>);
        static_assert(!retro::delegates::CanAddWeakLambda<FMultiGetObjectName, UObject *, void (UObject::*)(FString&) const>);
        static_assert(!retro::delegates::CanAddSP<FMultiGetObjectName, UObject *, void (UObject::*)(FString&) const>);
        static_assert(!retro::delegates::CanAddSPLambda<FMultiGetObjectName, UObject *,void (UObject::*)(FString&) const>);
        static_assert(retro::delegates::CanAddRaw<FMultiGetObjectName, UObject *, void (UObject::*)(FString&) const>);
        FMultiGetObjectName Delegate1;
        retro::delegates::Add(Delegate1, Object, static_cast<void (UObject::*)(FString&) const>(&UObject::GetName));
        FString Name;
        Delegate1.Broadcast(Name);
        CHECK(Name == Object->GetName());

        int Value = 12;
        auto Lambda = [&Value] { Value *= 2; };
        static_assert(retro::delegates::CanAddWeakLambda<FSimpleMulticastDelegate, UObject *, decltype(Lambda)>);
        static_assert(!retro::delegates::CanAddSPLambda<FSimpleMulticastDelegate, UObject *, decltype(Lambda)>);
        FSimpleMulticastDelegate Delegate2;
        retro::delegates::Add(Delegate2, Lambda);
        Delegate2.Broadcast();
        CHECK(Value == 24);
    }

    SECTION("Can bind to shared pointers") {
        auto SharedValue = MakeShared<FDemoClass>(12);
        static_assert(retro::delegates::CanAddSP<FMultiAddToValue, TSharedRef<FDemoClass> &, decltype(&FDemoClass::AddToValue)>);
        static_assert(retro::delegates::CanAddSP<FMultiAddToValue, FDemoClass *, decltype(&FDemoClass::AddToValue)>);

        FMultiAddToValue Delegate1;
        retro::delegates::Add(Delegate1, SharedValue, &FDemoClass::AddToValue);
        Delegate1.Broadcast(10);
        CHECK(SharedValue->GetValue() == 22);

        FMultiAddToValue Delegate2;
        retro::delegates::Add(Delegate2, &SharedValue.Get(), &FDemoClass::AddToValue);
        Delegate2.Broadcast(13);
        CHECK(SharedValue->GetValue() == 35);

        auto Lambda = [&RawValue = SharedValue.Get()](int32 Value) { RawValue.AddToValue(Value); };
        static_assert(retro::delegates::CanAddSPLambda<FMultiAddToValue, FDemoClass *, decltype(Lambda)>);
        FMultiAddToValue Delegate3;
        retro::delegates::Add(Delegate3, &SharedValue.Get(), Lambda);
        Delegate3.Broadcast(15);
        CHECK(SharedValue->GetValue() == 50);
    }

    SECTION("Can bind to raw pointers") {
        FUnsharedDemoClass RawValue(12);

        static_assert(retro::delegates::CanAddRaw<FMultiAddToValue, FUnsharedDemoClass *, decltype(&FUnsharedDemoClass::AddToValue)>);
        static_assert(
            !retro::delegates::CanAddUObject<FMultiAddToValue, FUnsharedDemoClass *, decltype(&FUnsharedDemoClass::AddToValue)>);
        static_assert(!retro::delegates::CanAddSP<FMultiAddToValue, FUnsharedDemoClass *, decltype(&FDemoClass::AddToValue)>);

        FMultiAddToValue Delegate1;
        retro::delegates::Add(Delegate1, &RawValue, &FUnsharedDemoClass::AddToValue);
        Delegate1.Broadcast(10);
        CHECK(RawValue.GetValue() == 22);
    }
}

#endif