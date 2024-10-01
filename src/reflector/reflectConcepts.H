#ifndef reflectConcepts_H
#define reflectConcepts_H

#include "autoPtr.H"
#include "dictionary.H"
#include "refl.H"
#include <string_view>
#include <type_traits>

namespace Foam {

/// @output_section author:elwardi
namespace Reflect {

/// @brief An alias to backend type list implementation
template <class... Ts> using typeList = refl::util::type_list<Ts...>;

/// @brief A trait for function pointers. Stolen off [this stackoverflow.com
/// answer](https://stackoverflow.com/questions/77210519/writing-a-c-concept-for-a-type-with-static-member-function-whose-arguments-sta/77214808#77214808)
template <typename> struct fnPtrTraits;

template <typename R, typename... Ps> struct fnPtrTraits<R (*)(Ps...)> {
    static constexpr bool isNoexcept = false;
    using resultType = R;
    using paramsType = typeList<Ps...>; // std::tuple<Ps...>;
};

template <typename R, typename... Ps>
struct fnPtrTraits<R (*)(Ps...) noexcept> : fnPtrTraits<R (*)(Ps...)> {
    static constexpr bool isNoexcept = true;
};

/// @brief An internal concept for having a typeName and a
/// dictionaryConstructorTablePtr.
template <class T>
concept DictionaryRTSModel = requires {
    { T::typeName } -> std::convertible_to<word>;
    {
        *T::dictionaryConstructorTablePtr_
    } -> std::convertible_to<typename T::dictionaryConstructorTableType>;
};

/// @brief A concept for base classes for RTS models which implement a
/// dictionary-based RTS and satisfies the following conditions:
/// - Is an abstract type
/// - Has a static New method which takes a dictionary as its first argument
/// @note For thisto work ::New static method for type T must have a dictionaty
/// as Its first argument, and never be overloaded...
template <class T>
concept DictionaryRTSModelBase = requires {
    requires DictionaryRTSModel<T>;
    []<typename Traits, typename R = Traits::resultType,
       typename Ps = Traits::paramsType>(Traits)
        requires std::same_as<R, autoPtr<T>> && (typeList<Ps>::size > 0) &&
                 std::same_as<std::remove_cvref_t<refl::trait::get_t<0, Ps>>,
                              dictionary>
    {}(fnPtrTraits<decltype(&T::New)>{});
    std::is_abstract_v<T>;
};

/// @brief A concept for concrete implementations of RTS Models to make sure a
/// type
/// - Is NOT abstract
/// - Its base class participates dictioanry-based RTS
template <class T, class B>
concept DictionaryRTSModelImpl = requires {
    std::is_abstract_v<T> == false;
    std::is_base_of_v<B, T>;
    requires DictionaryRTSModelBase<T>;
};

/// @brief A concept for classes supporting self-reflection capabilities.
template <class T>
concept SelfReflectableModel = requires(word w) {
    { T::schema(w) } -> std::convertible_to<dictionary>;
};

/// @brief An innacurate convert for OpenFOAM pointers.
/// @todo incomplete concept
template <class T>
concept SomePtr = requires(T t) {
    { t.ref() };
    { t.get() };
    { t.ptr() };
};

/// @brief A concept for class members which get maximum support
/// from the reflection system
template <typename T>
concept UIElement = requires {
    typename T::valueType;
    { T::v } -> std::convertible_to<typename T::valueType>;
    { T::init() } -> std::convertible_to<typename T::valueType>;
    { T::descr() } -> std::convertible_to<std::string_view>;
};

/// @brief A concept for class members which get maximum support
/// from the reflection system that come with a minimal value
template <typename T>
concept UIElementWithMin = requires {
    UIElement<T>;
    { T::min() } -> std::convertible_to<typename T::valueType>;
};

/// @brief A concept for class members which get maximum support
/// from the reflection system that come with a maximal value
template <typename T>
concept UIElementWithMax = requires {
    UIElement<T>;
    { T::max() } -> std::convertible_to<typename T::valueType>;
};

/// @brief A concept for wrappers of RTS classes
template <typename T>
concept WrapperDictionaryRTSBase = requires {
    DictionaryRTSModelBase<typename T::wrappedType>;
};
template <typename T, typename U>
concept WrapperDictionaryRTSImpl = requires {
    DictionaryRTSModelImpl<typename T::wrappedType, typename U::wrappedType>;
};


/// @breif A concept for non-reflectable types
template <typename T>
concept Reflectable = SomePtr<T> || refl::trait::is_reflectable_v<T>;

template <typename T>
concept NonReflectableRTSModel = !(Reflectable<T>) && DictionaryRTSModel<T>;

} // namespace Reflect

} // namespace Foam

#endif
