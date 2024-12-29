﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/UObjectIterator.h>

namespace retro::ranges {
	
	template <std::derived_from<UObject> T>
	class TObjectView {
		struct FIterator {
			using value_type = T*;
			using difference_type = std::ptrdiff_t;

			FIterator() = default;

			FIterator(const FIterator&) = delete;
			FIterator(FIterator&&) = default;

			~FIterator() = default;
			
			FIterator& operator=(const FIterator&) = delete;
			FIterator& operator=(FIterator&&) = default;

			T* operator*() const {
				return *Source;
			}

			T* operator->() const {
				return *Source;
			}

			bool operator==(const std::default_sentinel_t&) const {
				return static_cast<bool>(Source);
			}

			FIterator &operator++() {
				++Source;
				return *this;
			}

			void operator++(int) {
				++Source;
			}

		private:
			TObjectIterator<T> Source;
		};
		
	public:
		TObjectView() = default;

		FIterator begin() const {
			return FIterator();
		}

		std::default_sentinel_t end() const {
			return std::default_sentinel_t();
		}
		
	};
}
