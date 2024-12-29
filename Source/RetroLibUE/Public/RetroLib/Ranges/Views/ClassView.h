#pragma once

#include <UObject/UObjectIterator.h>
#include "RetroLib/Concepts/Interfaces.h"

namespace retro::ranges {
	
	template <typename T>
		requires std::derived_from<T, UObject> || UnrealInterface<T>
	class TClassView {
		struct FIterator {
			using value_type = TSubclassOf<T>;
			using difference_type = std::ptrdiff_t;

			FIterator() = default;

			FIterator(const FIterator&) = delete;
			FIterator(FIterator&&) = default;

			~FIterator() = default;
			
			FIterator& operator=(const FIterator&) = delete;
			FIterator& operator=(FIterator&&) = default;

			TSubclassOf<T> operator*() const {
				return *Source;
			}

			UClass* operator->() const {
				return *Source;
			}

			bool operator==(const std::default_sentinel_t&) const {
				return static_cast<bool>(Source);
			}

			FIterator &operator++() requires std::derived_from<T, UObject> {
				while (true) {
					++Source;
					if (!Source || Source->IsChildOf<T>()) {
						break;
					}
				}
				
				return *this;
			}

			FIterator &operator++() requires UnrealInterface<T> {
				while (true) {
					++Source;
					if (!Source || Source->ImplementsInterface(typename T::UClassType::StaticClass())) {
						break;
					}
				}
				
				return *this;
			}

			void operator++(int) {
				++*this;
			}

		private:
			TObjectIterator<UClass> Source;
		};
		
	public:
		TClassView() = default;

		FIterator begin() const {
			return Iterator();
		}

		std::default_sentinel_t end() const {
			return std::default_sentinel_t();
		}

		
	};
}
