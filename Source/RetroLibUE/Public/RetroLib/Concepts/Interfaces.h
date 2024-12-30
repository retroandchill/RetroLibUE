﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

namespace Retro {
	/**
	 * Used to check if the given type is an Unreal Interface
	 * @param T The particular interface type
	 */
	template <typename T>
	concept UnrealInterface = !std::is_base_of_v<UObject, T> && std::same_as<typename T::ThisClass, T> &&
							  std::is_base_of_v<UInterface, typename T::UClassType> && requires(T &&Interface) {
		{ Interface._getUObject() } -> std::same_as<UObject *>;
							  };
}