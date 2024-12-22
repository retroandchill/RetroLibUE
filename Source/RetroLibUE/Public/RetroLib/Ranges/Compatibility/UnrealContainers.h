// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RetroLib/Ranges/Concepts/Containers.h"
#include "RetroLib/Ranges/Compatibility/ForEachRange.h"

#include "Traits/IsContiguousContainer.h"

namespace retro::ranges {

	template <typename C, typename T>
	concept UnrealEmplace = requires(C &Container, T&& Ref) {
		Container.Emplace(std::forward<T>(Ref));
	};

	template <typename C, typename T>
	concept UnrealAdd = requires(C &Container, T&& Ref) {
		Container.Add(std::forward<T>(Ref));
	};

	template <typename C, typename T>
	concept UnrealInsert = requires(C &Container, T&& Ref) {
		Container.Insert(std::forward<T>(Ref));
	};

	template <typename C, typename T>
	concept UnrealAddable = requires(C &Container, T&& Ref) {
		Container += std::forward<T>(Ref);
	};

	template <typename C, typename T>
	concept UnrealAppendable = UnrealEmplace<C, T> || UnrealAdd<C, T> || UnrealInsert<C, T> || UnrealAddable<C, T>;

	template <typename C>
	concept UnrealSizedContainer = std::ranges::range<C> && requires(C &Container) {
		{ Container.Num() } -> std::convertible_to<int32>;
	};

	template <typename C>
	concept UnrealSizedString = std::ranges::range<C> && requires(C &Container) {
		{ Container.Len() } -> std::convertible_to<int32>;
	};

	template <typename T>
	concept UnrealReservable = std::ranges::sized_range<T> && requires(T &container, std::ranges::range_size_t<T> size) {
		container.Reserve(size);
		container.Max();
	};

	template <typename T>
	concept UnrealStringReservable = std::ranges::sized_range<T> && requires(T &container, std::ranges::range_size_t<T> size) {
		container.Reserve(size);
		container.GetAllocatedSize();
	};

	
}

template <retro::ranges::UnrealSizedContainer R>
constexpr auto size(const R& Range) {
	return Range.Num();
}

template <retro::ranges::UnrealSizedString R>
constexpr auto size(const R& Range) {
	return Range.Len();
}

template <retro::ranges::CanBridgeToRange I>
constexpr auto begin(I &range) {
	return retro::ranges::AdapterIterator<retro::IteratorType<I>, retro::SentinelType<I>>(range.begin());
}

template <retro::ranges::CanBridgeToRange I>
constexpr auto end(I &range) {
	return retro::ranges::SentinelAdapter<retro::IteratorType<I>, retro::SentinelType<I>>(range.end());
}

namespace retro::ranges {
	template <typename C>
		requires UnrealAppendable<C, std::ranges::range_value_t<C>>
	struct AppendableContainerType<C> : ValidType {
		template <typename T>
			requires retro::ranges::UnrealAppendable<C, T>
		static constexpr decltype(auto) append(C& Container, T &&Value) {
			if constexpr (retro::ranges::UnrealEmplace<C, T>) {
				return Container.Emplace(std::forward<T>(Value));
			} else if constexpr (retro::ranges::UnrealAdd<C, T>) {
				return Container.Add(std::forward<T>(Value));
			} else if constexpr (retro::ranges::UnrealInsert<C, T>) {
				return Container.Insert(std::forward<T>(Value));
			} else if constexpr (retro::ranges::UnrealAddable<C, T>) {
				return Container += std::forward<T>(Value);
			}
		}
	};
	
	template <UnrealReservable T>
	struct ReservableContainerType<T> : ValidType {
		static constexpr void reserve(T& Container, int32 Size) {
			Container.Reserve(Size);
		}

		static constexpr int32 capacity(const T& Container) {
			return Container.Max();
		}

		static constexpr int32 max_size(const T& Container) {
			return std::numeric_limits<decltype(Container.Max())>::max();
		}
	};

	template <UnrealStringReservable T>
	struct ReservableContainerType<T> : ValidType {
		static constexpr void reserve(T& Container, int32 Size) {
			Container.Reserve(Size);
		}

		static constexpr int32 capacity(const T& Container) {
			return Container.GetAllocatedSize() / sizeof(typename T::ElementType);
		}

		static constexpr int32 max_size(const T& Container) {
			return std::numeric_limits<decltype(Container.GetAllocatedSize())>::max();
		}
	};
}