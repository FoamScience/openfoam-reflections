#ifndef uiConcepts_H
#define uiConcepts_H

#include "refl.H"
#include "dictionary.H"
#include "autoPtr.H"
#include <type_traits>

namespace Foam
{

namespace ui
{

template<class... Ts>
using typeList = refl::util::type_list<Ts...>;

/// @brief A trait for function pointers. Stolen off [this stackoverflow.com answer](https://stackoverflow.com/questions/77210519/writing-a-c-concept-for-a-type-with-static-member-function-whose-arguments-sta/77214808#77214808)
/// @hyde-owner elwardi
template<typename>
struct fnPtrTraits;

template<typename R, typename ...Ps>
struct fnPtrTraits<R(*)(Ps...)> {
    static constexpr bool isNoexcept = false;
    using resultType = R;
    using paramsType = typeList<Ps...>;//std::tuple<Ps...>;
};

template<typename R, typename ...Ps>
struct fnPtrTraits<R(*)(Ps...) noexcept> : fnPtrTraits<R(*)(Ps...)> {
    static constexpr bool isNoexcept = true;
};

/// @brief An internal concept for having a typeName and a dictionaryConstructorTablePtr.
/// @hyde-owner elwardi
template<class T>
concept DictionaryRTSModel = requires {
    { T::typeName } -> std::convertible_to<word>;
    { *T::dictionaryConstructorTablePtr_ }
    -> std::convertible_to<typename T::dictionaryConstructorTableType>;
};

/// @brief A concept for base classes for RTS models
/// @hyde-owner elwardi
template<class T>
concept DictionaryRTSModelBase = requires {
    requires DictionaryRTSModel<T>;
    []<typename Traits, typename R = Traits::resultType, typename Ps = Traits::paramsType>(Traits)
    requires std::same_as<R, autoPtr<T>>
          && (typeList<Ps>::size > 0)
          && std::same_as<std::remove_cvref_t<refl::trait::get_t<0, Ps>>, dictionary>
    { }(fnPtrTraits<decltype(&T::New)>{});
    std::is_abstract_v<T>;
};

/// @brief A concept for concrete implementations of RTS Models
/// @hyde-owner elwardi
template<class T, class B>
concept DictionaryRTSModelImpl = requires {
    std::is_abstract_v<T> == false;
    requires DictionaryRTSModel<T>;
    []<typename Traits, typename R = Traits::resultType, typename Ps = Traits::paramsType>(Traits)
    requires std::same_as<R, autoPtr<B>>
          && (typeList<Ps>::size > 0)
          && std::same_as<std::remove_cvref_t<refl::trait::get_t<0, Ps>>, dictionary>
    { }(fnPtrTraits<decltype(&T::New)>{});
    std::is_base_of_v<B, T>;
};

/// @brief A concept for OpenFOAM classes with self-reflection capabilities.
/// @hyde-owner elwardi
template<class T>
concept SelfReflectableModel = requires(word w) {
    { T::schema(w) } -> std::convertible_to<dictionary>;
};

/// @brief An innacurate convert for OpenFOAM pointers.
/// @todo incomplete concept
/// @hyde-owner elwardi
template<class T>
concept SomePtr = requires (T t) {
    { t.ref() };
    { t.get() };
};

template<class T>
concept DefaultedMetadataType = requires {
    {T::default_};
    {T::name_};
    {T::description_};
};

} /// !namespace ui

} /// !namespace Foam

#endif
