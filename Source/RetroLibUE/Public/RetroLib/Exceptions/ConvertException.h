// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/BlueprintExceptionInfo.h>
#include <exception>

namespace Retro {
	inline FBlueprintExceptionInfo ConvertException(const std::exception& Exception, EBlueprintExceptionType::Type Type = EBlueprintExceptionType::AccessViolation) {
		return FBlueprintExceptionInfo(Type, FText::FromString(Exception.what()));
	}
}
