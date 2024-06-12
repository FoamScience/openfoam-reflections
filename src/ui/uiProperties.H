#ifndef uiProperties_H
#define uiProperties_H

#include "refl.H"
#include "word.H"
#include "error.H"
#include <type_traits>

namespace Foam
{

namespace ui
{

template<class T>
struct defaulted : public refl::attr::usage::field
{
    const T value;
    constexpr defaulted(T val) noexcept
        : value(val)
    {
    }
};

template<class T>
struct properties {

    template <typename MemberDescriptor>
    static constexpr bool isDefaulted(MemberDescriptor) noexcept
    {
        static_assert(refl::trait::is_field_v<MemberDescriptor>);
        return refl::trait::contains_v
            <
                defaulted<typename MemberDescriptor::value_type>,
                typename MemberDescriptor::attribute_types
            >;
    };

    template<class FieldDescriptor>
    static constexpr T getDefault(FieldDescriptor d) noexcept
    {
        static_assert(refl::trait::is_field_v<FieldDescriptor>);
        return refl::descriptor::get_attribute<defaulted<T>>(d).value;
    }   

    template<class U>
    static T getMemberDefault(const std::string& name) {
        using pureTypeName = std::remove_cvref_t<U>;
        static_assert(refl::trait::is_reflectable_v<pureTypeName>, "Unsupported type!");
        constexpr auto type = refl::reflect<pureTypeName>();
        std::optional<T> result{};
        constexpr auto defaultedMembers = filter(type.members, [](auto member) { return isDefaulted(member); });
        for_each(defaultedMembers, [&](auto member, auto index) {
            constexpr std::string_view memberName = member.name.c_str();
            if (memberName == name)
            {
                result = refl::util::get_instance<defaulted>(member.attributes).value;
            }
        });
        // Missing error handling
        return *result;
    }
};



} /// !namespace ui

} /// !namespace Foam

#endif
