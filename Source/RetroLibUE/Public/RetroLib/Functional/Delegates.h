// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RetroLib/Concepts/Delegates.h"
#include "RetroLib/Functional/CreateBinding.h"
#include "RetroLib/Utils/Unreachable.h"

namespace retro {
    template <UEDelegate D>
    struct TDelegateInvoker {

        template <typename T>
            requires std::constructible_from<D, T> && (!std::same_as<std::decay_t<T>, TDelegateInvoker>)
        constexpr explicit TDelegateInvoker(T &&Delegate) : Delegate(std::forward<T>(Delegate)) {
        }

        template <typename... A>
        constexpr decltype(auto) operator()(A &&...Args) const {
            if constexpr (UnicastDelegate<D>) {
                check(Delegate.IsBound()) return Delegate.Execute(std::forward<A>(Args)...);
            } else if constexpr (MulticastDelegate<D>) {
                return Delegate.Broadcast(std::forward<A>(Args)...);
            }
        }

      private:
        D Delegate;
    };

    template <UEDelegate D>
    TDelegateInvoker(D &&) -> TDelegateInvoker<std::decay_t<D>>;

    template <UEDelegate D>
    struct AdditionalBindingTypes<D> : ValidType {
        template <UEDelegate F, typename... A>
            requires std::same_as<D, std::decay_t<F>>
        static constexpr auto bind(F &&Delegate, A &&...Args) {
            return retro::bind_back(TDelegateInvoker(std::forward<F>(Delegate)), std::forward<A>(Args)...);
        }
    };

    template <UEDelegate D, typename F, typename... A>
        requires CanBindFree<D, F, A...>
    D CreateDelegate(F &&Functor, A &&...Args) {
        if constexpr (CanBindStatic<D, F, A...>) {
            return D::CreateStatic(std::forward<F>(Functor), std::forward<A>(Args)...);
        } else {
            static_assert(CanBindLambda<D, F, A...>);
            return D::CreateLambda(std::forward<F>(Functor), std::forward<A>(Args)...);
        }
    }

    template <UEDelegate D, typename O, typename F, typename... A>
        requires CanBindMember<D, O, F, A...>
    D CreateDelegate(O &&Object, F &&Functor, A &&...Args) {
        if constexpr (CanBindSP<D, O, F, A...>) {
            return D::CreateSP(std::forward<O>(Object), std::forward<F>(Functor), std::forward<A>(Args)...);
        } else if constexpr (CanBindSPLambda<D, O, F, A...>) {
            return D::CreateSPLambda(std::forward<O>(Object), std::forward<F>(Functor), std::forward<A>(Args)...);
        } else if constexpr (CanBindUObject<D, O, F, A...>) {
            return D::CreateUObject(std::forward<O>(Object), std::forward<F>(Functor), std::forward<A>(Args)...);
        } else if constexpr (CanBindWeakLambda<D, O, F, A...>) {
            return D::CreateWeakLambda(std::forward<O>(Object), std::forward<F>(Functor), std::forward<A>(Args)...);
        } else {
            static_assert(CanBindRaw<D, O, F, A...>);
            return D::CreateRaw(std::forward<O>(Object), std::forward<F>(Functor), std::forward<A>(Args)...);
        }
    }

} // namespace retro
