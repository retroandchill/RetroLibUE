// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/UObjectIterator.h>

namespace retro::ranges {
	
	template <std::derived_from<UObject> T>
	class TObjectView {
		struct Iterator {
			using value_type = T*;
			using difference_type = std::ptrdiff_t;

			T* operator*() const {
				return *Source;
			}

			T* operator->() const {
				return *Source;
			}

			bool operator==(const std::default_sentinel_t&) const {
				return static_cast<bool>(Source);
			}

			Iterator &operator++() {
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

		Iterator begin() const {
			return Iterator();
		}

		std::default_sentinel_t end() const {
			return std::default_sentinel_t();
		}
		
	};
}
