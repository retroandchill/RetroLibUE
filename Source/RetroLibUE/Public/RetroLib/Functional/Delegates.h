// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RetroLib/Functional/CreateBinding.h"
#include "RetroLib/Concepts/Delegates.h"

namespace retro {
	template <UEDelegate D>
	struct TDelegateInvoker {

		template <typename T>
			requires std::constructible_from<D, T> && (!std::same_as<std::decay_t<T>, TDelegateInvoker>)
		constexpr explicit TDelegateInvoker(T&& Delegate) : D(std::forward<T>(Delegate)) {}

		template <typename... A>
		constexpr decltype(auto) operator()(A&&... Args) const {
			if constexpr (UnicastDelegate<D>) {
				check(Delegate.IsBound())
				return Delegate.Execute(std::forward<A>(Args)...);
			} else if constexpr (MulticastDelegate<D>) {
				return Delegate.Broadcast(std::forward<A>(Args)...);
			}
		}
		
	private:
		D Delegate;
	};

	template <UEDelegate D>
	TDelegateInvoker(D) -> TDelegateInvoker<std::decay_t<D>>;
}

template <retro::UEDelegate D, typename... A>
constexpr decltype(auto) create_binding(D&& Delegate, A&&... Args) {
	return retro::create_binding(retro::TDelegateInvoker(std::forward<D>(Delegate)), std::forward<A>(Args)...);
}