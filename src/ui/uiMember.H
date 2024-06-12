#ifndef uiMember_H
#define uiMember_H

#include <concepts>
#include <string_view>

namespace Foam {

namespace ui {

/// @brief class members with reflection support
/// @hyde-owner elwardi
template<class T,
         auto InitFunc,
         auto DescFunc,// = []() { return "No description"; },
         auto MinFunc = nullptr,
         auto MaxFunc = nullptr>
struct member
{
    /// @brief type of member
    using value_type = T;

    /// @brief member type
    static constexpr T init() { return InitFunc(); }

    /// @brief member description
    static constexpr std::string_view descr() { return DescFunc(); }

    /// @brief member value
    T v;

    /// @brief member min
    template<typename U = T>
        requires(MinFunc != nullptr &&
                 std::convertible_to<decltype(MinFunc()), U>)
    static constexpr auto min() -> U
    {
        return MinFunc();
    }

    /// @brief member max
    template<typename U = T>
        requires(MaxFunc != nullptr &&
                 std::convertible_to<decltype(MaxFunc()), U>)
    static constexpr auto max() -> U
    {
        return MaxFunc();
    }

    /// @brief member name
    operator T() const { return v; }
};

template <typename T>
concept isUIMember = requires {
    //typename T::value_type;
    { T::v } -> std::convertible_to<typename T::value_type>;
    { T::init() } -> std::convertible_to<typename T::value_type>;
    { T::descr() } -> std::convertible_to<std::string_view>;
};

} /// !namespace ui

} /// !namespace Foam

#endif
