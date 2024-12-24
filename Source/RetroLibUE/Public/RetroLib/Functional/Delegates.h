// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RetroLib/Functional/CreateBinding.h"
#include "RetroLib/Concepts/Delegates.h"

namespace retro {
	template <UEDelegate D>
	struct TDelegateInvoker {

		template <typename T>
			requires std::constructible_from<D, T> && (!std::same_as<std::decay_t<T>, TDelegateInvoker>)
		constexpr explicit TDelegateInvoker(T&& Delegate) : Delegate(std::forward<T>(Delegate)) {}

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
	TDelegateInvoker(D&&) -> TDelegateInvoker<std::decay_t<D>>;

	template <UEDelegate D>
	struct AdditionalBindingTypes<D> : ValidType {
		template <UEDelegate F, typename... A>
			requires std::same_as<D, std::decay_t<F>>
		static constexpr auto bind(F&& Delegate, A&&... Args) {
			return retro::bind_back(TDelegateInvoker(std::forward<F>(Delegate)), std::forward<A>(Args)...);
		}
	};
}