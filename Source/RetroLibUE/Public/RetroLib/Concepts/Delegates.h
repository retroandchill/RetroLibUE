// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

namespace retro {
	/**
     * Concept to check if a delegate is a native (single binding) delegate.
     * 
     * @tparam T The type to check if it's a delegate or not
     */
    template <typename T>
    concept NativeUnicastDelegate = requires(T &&Delegate) {
        { TDelegate(std::forward<T>(Delegate)) } -> std::same_as<std::remove_cvref_t<T>>;
    };

    /**
     * Concept to check if a delegate is a dynamic (single binding) delegate.
     * 
     * @tparam T The type to check if it's a delegate or not
     */
    template <typename T>
    concept DynamicUnicastDelegate =
        std::is_base_of_v<decltype(TScriptDelegate(std::declval<T>())), std::remove_cvref_t<T>>;

    /**
     * Concept to check if a delegate is a (single binding) delegate.
     * 
     * @tparam T The type to check if it's a delegate or not
     */
    template <typename T>
    concept UnicastDelegate = NativeUnicastDelegate<T> || DynamicUnicastDelegate<T>;

    /**
     * Concept to check if a delegate is a native multicast delegate.
     * 
     * @tparam T The type to check if it's a delegate or not
     */
    template <typename T>
    concept NativeMulitcastDelegate = requires(T &&Delegate) {
        { TMulticastDelegate(std::forward<T>(Delegate)) } -> std::same_as<std::remove_cvref_t<T>>;
    };

    /**
     * Concept to check if a delegate is a dynamic multicast delegate.
     * 
     * @tparam T The type to check if it's a delegate or not
     */
    template <typename T>
    concept DynamicMulitcastDelegate =
        std::is_base_of_v<decltype(TMulticastScriptDelegate(std::declval<T>())), std::remove_cvref_t<T>>;

    /**
     * Concept to check if a delegate is a multicast delegate.
     * 
     * @tparam T The type to check if it's a delegate or not
     */
    template <typename T>
    concept MulticastDelegate = NativeMulitcastDelegate<T> || DynamicMulitcastDelegate<T>;

    template <typename T>
    concept NativeDelegate = NativeUnicastDelegate<T> || NativeMulitcastDelegate<T>;

    template <typename T>
    concept DynamicDelegate = DynamicUnicastDelegate<T> || DynamicMulitcastDelegate<T>;

    /**
     * Concept to check if a delegate is a Unreal Engine delegate type
     * 
     * @tparam T The type to check if it's a delegate or not
     */
    template <typename T>
    concept UEDelegate = UnicastDelegate<T> || MulticastDelegate<T>;

    /**
     * Concept to check if a single-cast delegate is bindable to a target multicast delegate
     */
    template <typename M, typename S>
    concept BindableTo = MulticastDelegate<M> && UnicastDelegate<S> && requires(M &&Delegate, S &&Source) {
        { Delegate.Add(std::forward<S>(Source)) } -> std::same_as<FDelegateHandle>;
    };
}