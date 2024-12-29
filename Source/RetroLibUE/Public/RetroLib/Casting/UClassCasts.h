// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RetroLib/Casting/ClassCast.h"
#include "RetroLib/Concepts/Interfaces.h"

namespace retro {
	template <std::derived_from<UObject> T>
	struct InstanceOf<T> {
		
	};

	template <std::derived_from<UObject> T>
	struct InstanceChecker<T> {
		/**
		 * Checks if the given instance of type U is a valid instance of the desired base type T.
		 *
		 * @param Value The instance of type U to be checked for validity.
		 * @return True if the instance is valid, meaning that U is derived from T or is an instance
		 *         that can be dynamically cast to T. Otherwise, returns false.
		 */
		template <std::derived_from<UObject> U>
		constexpr bool operator()(const U& Value) const {
			if constexpr (std::derived_from<U, T>) {
				// Trivial case, U is derived from T, so we know with certainty that this is valid
				return true;
			} else {
				return Value.template IsA<T>();
			}
		}

		template <UnrealInterface U>
		constexpr bool operator()(const U& Value) const {
			if constexpr (std::derived_from<U, T>) {
				// Trivial case, U is derived from T, so we know with certainty that this is valid
				return true;
			} else {
				check(Value._getUObject() != nullptr);
				return Value._getUObject()->template IsA<T>();
			}
		}
	};

	template <UnrealInterface T>
	struct InstanceChecker<T> {
		/**
		 * Checks if the given instance of type U is a valid instance of the desired base type T.
		 *
		 * @param Value The instance of type U to be checked for validity.
		 * @return True if the instance is valid, meaning that U is derived from T or is an instance
		 *         that can be dynamically cast to T. Otherwise, returns false.
		 */
		template <std::derived_from<UObject> U>
		constexpr bool operator()(const U& Value) const {
			if constexpr (std::derived_from<U, T>) {
				// Trivial case, U is derived from T, so we know with certainty that this is valid
				return true;
			} else {
				return Value.template Implements<typename T::UClassType>();
			}
		}

		template <UnrealInterface U>
		constexpr bool operator()(const U& Value) const {
			if constexpr (std::derived_from<U, T>) {
				// Trivial case, U is derived from T, so we know with certainty that this is valid
				return true;
			} else {
				check(Value._getUObject() != nullptr);
				return Value._getUObject()->template Implements<typename T::UClassType>();
			}
		}
	};
}
