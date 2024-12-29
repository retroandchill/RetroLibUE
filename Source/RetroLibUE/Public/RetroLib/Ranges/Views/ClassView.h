#pragma once

#include <UObject/UObjectIterator.h>
#include "RetroLib/Concepts/Interfaces.h"

namespace retro::ranges {
	
	template <typename T>
		requires std::derived_from<T, UObject> || UnrealInterface<T>
	class TClassView {
		struct Iterator {
			using value_type = TSubclassOf<T>;
			using difference_type = std::ptrdiff_t;

			TSubclassOf<T> operator*() const {
				return *Source;
			}

			UClass* operator->() const {
				return *Source;
			}

			bool operator==(const std::default_sentinel_t&) const {
				return static_cast<bool>(Source);
			}

			Iterator &operator++() requires std::derived_from<T, UObject> {
				while (true) {
					++Source;
					if (!Source || Source->IsChildOf<T>()) {
						break;
					}
				}
				
				return *this;
			}

			Iterator &operator++() requires UnrealInterface<T> {
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

		Iterator begin() const {
			return Iterator();
		}

		std::default_sentinel_t end() const {
			return std::default_sentinel_t();
		}

		
	};
}
