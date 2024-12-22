// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RetroLib/Optionals/OptionalOperations.h"
#include "RetroLib/Utils/ForwardLike.h"

namespace retro::optionals {
	template <typename T>
	concept UnrealOptional = requires(T&& Optional) {
		Optional.GetValue();
		*Optional;
		Optional.operator->();
		{ Optional.IsSet() } -> std::same_as<bool>;
	};

    template <UnrealOptional T>
    struct OptionalOperations<T> : ValidType {
        template <UnrealOptional O>
            requires std::same_as<T, std::decay_t<O>>
        static constexpr decltype(auto) get(O&& Optional) {
            return retro::forward_like<O>(*Optional);
        }

        template <UnrealOptional O>
            requires std::same_as<T, std::decay_t<O>>
        static constexpr decltype(auto) get_value(O&& Optional) {
            return retro::forward_like<O>(Optional.GetValue());
        }

        template <UnrealOptional O>
            requires std::same_as<T, std::decay_t<O>>
        static constexpr bool has_value(const O& Optional) {
            return Optional.IsSet();
        }
    };
}

/**
 * Template specialization for an optional that takes in a reference.
 * @tparam T The type of the optional that was consumed.
 */
template <typename T>
struct TOptional<T &> {

    /**
     * Typedef for the element type used by this container.
     */
    using ElementType = T;

    /**
     * Default constructor, creates an empty optional.
     */
    constexpr TOptional() = default;

    /**
     * Construct a new optional from a reference.
     * @param Value The value to reference in this optional.
     */
    constexpr explicit(false) TOptional(T &Value) : Data(&Value) {
    }

    /**
     * Construct a new optional from a nullable pointer.
     * @param Value
     */
    constexpr explicit(false) TOptional(T *Value) : Data(Value) {
    }

    /**
     * Constructor from nullptr
     */
    constexpr explicit(false) TOptional(nullptr_t) {
    }

    /**
     * Construct a new optional from an optional of subclass.
     * @tparam U The subclass type
     * @param Other The other optional
     */
    template <typename U>
        requires std::derived_from<U, T> && (!std::is_same_v<T, U>)
    constexpr explicit(false) TOptional(TOptional<U &> Other) : Data(Other.GetPtrOrNull()) {
    }

    /**
     * Assignment operator from the underlying value.
     * @param Value The value to assign in.
     * @return A reference to this object
     */
    TOptional &operator=(T &Value) {
        Data = &Value;
        return *this;
    }

    /**
     * Emplace a new reference value into the optional.
     * @param Value The value to emplace
     * @return A reference to the emplaced value
     */
    T &Emplace(T &Value) {
        Data = Value;
        return *Data;
    }

    /**
     * Get the value of the optional.
     * @param DefaultValue The default to substitute for an empty optional.
     * @return The retrieved value
     */
    T &Get(T &DefaultValue) const
        requires(!std::is_const_v<T>)
    {
        return Data != nullptr ? *Data : DefaultValue;
    }

    /**
     * Get the value of the optional.
     * @param DefaultValue The default to substitute for an empty optional.
     * @return The retrieved value
     */
    const T &Get(const T &DefaultValue) const {
        return Data != nullptr ? *Data : DefaultValue;
    }

    /**
     * Get a nullable pointer for the optional.
     * @return The retrieved value
     */
    T *GetPtrOrNull() const {
        return Data;
    }

    /**
     * Get a reference to the underlying data.
     * @return The retrieved value
     */
    T &GetValue() const {
        check(Data != nullptr)
        return *Data;
    }

    /**
     * Returns if the value is set
     * @return Is there a valid optional?
     */
    bool IsSet() const {
        return Data != nullptr;
    }

    /**
     * Reset the data to empty.
     */
    void Reset() {
        Data = nullptr;
    }

    /**
     * Dereference operator.
     * @return A reference to the underlying data.
     */
    T &operator*() {
        check(Data != nullptr)
        return *Data;
    }

    /**
     * Dereference operator.
     * @return A reference to the underlying data.
     */
    T &operator*() const {
        check(Data != nullptr)
        return *Data;
    }

    /**
     * Pointer member access operator.
     * @return A pointer to the underlying data.
     */
    T *operator->() {
        check(Data != nullptr)
        return Data;
    }

    /**
     * Pointer member access operator.
     * @return A pointer to the underlying data.
     */
    T *operator->() const {
        check(Data != nullptr)
        return Data;
    }

  private:
    T *Data = nullptr;
};

template <>
struct retro::optionals::IsRawReferenceOptionalAllowed<TOptional> : std::true_type {};