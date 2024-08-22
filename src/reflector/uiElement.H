#ifndef uiElement_H
#define uiElement_H

#include "dictionary.H"
#include <concepts>
#include <iostream>
#include <string_view>

namespace Foam {

namespace Reflect {

/// @brief class members with full reflection support
/// @output_section author:Elwardi
template <class T, auto InitFunc,
          auto DescFunc, // = []() { return "No description"; },
          auto MinFunc = nullptr, auto MaxFunc = nullptr>
struct uiElement {
    /// @brief actual type of member
    using valueType = T;

    /// @brief default value for member
    /// @returns a T, as value
    /// @note It seems returning an auto will cause problems with type
    /// compatibility
    static constexpr T init() {
        // if constexpr (std::is_convertible_v<decltype(InitFunc()), T>) {
        return T(InitFunc());
        //} else if constexpr (std::is_convertible_v<decltype(T(InitFunc())),
        //T>) {
        //    return T(InitFunc());
        //} else {
        //    //static_assert(false, "T cannot be constructed from the return
        //    type of InitFunc().");
        //}
    }

    /// @brief member description
    static constexpr std::string_view descr() { return DescFunc(); }

    /// @brief member value
    T v;

    /// @brief default constructor
    uiElement() : v(init()) {}

    /// @brief construct from dict value using default
    uiElement(const dictionary &c, const word &key)
        : v(c.getOrDefault<T>(key, init())) {}

    /// @brief construct from returned value if Pred is true, otherwise from
    /// default
    template <class Ret, class Pred>
        requires std::invocable<Pred> &&
                 std::convertible_to<std::invoke_result_t<Pred>, bool>
    uiElement(Ret ret, Pred pred) : v(pred() ? ret() : init()) {}

    /// @brief member min
    template <typename U = T>
        requires(MinFunc != nullptr &&
                 std::convertible_to<decltype(MinFunc()), U>)
    static constexpr auto min() -> U {
        return MinFunc();
    }

    /// @brief member max
    template <typename U = T>
        requires(MaxFunc != nullptr &&
                 std::convertible_to<decltype(MaxFunc()), U>)
    static constexpr auto max() -> U {
        return MaxFunc();
    }

    /// @brief convert to T
    operator T() const { return v; }
};

#define withDefault(initExpr) []() { return initExpr; }

#define withDefaultCaptuting(initExpr, captures)                               \
    [&captures]() { return initExpr; }

#define withDescription(descExpr) []() { return descExpr; }

#define withMin(min) []() { return min; }

#define withMax(max) []() { return max; }

} // namespace Reflect

} // namespace Foam

#endif
