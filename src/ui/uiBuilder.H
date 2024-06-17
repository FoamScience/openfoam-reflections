#ifndef uiBuilder_H
#define uiBuilder_H

#include "HashTable.H"
#include "dictionary.H"
#include "uiConcepts.H"
#include "uiMember.H"
#include "uiProperties.H"
#include <cstdlib>
#include <memory>
#include <string>
#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace Foam {

namespace ui {

/// @brief Utilities for looping over members of classes
/// @hyde-owner elwardi
template<class T>
struct builder
{
    /// @brief A function to return a familiar name from a C++ type name
    /// @return A string representing the type
    static word familiarNaming(word name)
    {
        HashTable<word> map;
        map.insert("std::__cxx11::basic_string<char, std::char_traits<char>, "
                   "std::allocator<char> >",
                   "std::string");
        map.insert("long", "long integer");
        if (map.found(name))
            return map[name];
        return name;
    }

    /// @breif A function to demangle types from typeid.
    /// Only compiled for Gcc, other compilers will display mangled names
    /// @hyde-owner elwardi
    /// @return
    static word demangle(const char* name)
    {
#ifdef __GNUG__
        int status = -4;
        std::unique_ptr<char, void (*)(void*)> res{
            abi::__cxa_demangle(name, NULL, NULL, &status), std::free
        };
        return (status == 0) ? res.get() : name;
#else
        return name;
#endif
    }

    template<DictionaryRTSModelBase U>
    static string getRTSOptions()
    {
        string options = "__one of ( ";
        for (auto& e : U::dictionaryConstructorTablePtr_->toc()) {
            options += e + " ";
        }
        options += " )__";
        return options;
    }

    template<DictionaryRTSModelBase U>
    static std::vector<string> getRTSJSONOptions()
    {
        std::vector<string> opts{""};
        for (const auto& e:  U::dictionaryConstructorTablePtr_->toc()) {
            opts.push_back(e);
        }
        return opts;
    }

    /// @brief A function to build a dictionary from reflected class members
    static dictionary schema()
    {
        auto processKey = [](word key) {
            key.removeEnd('_');
            return key;
        };
        dictionary dict;
        using pureTypeName = std::remove_cvref_t<T>;
        constexpr auto type = refl::reflect<pureTypeName>();
        if constexpr (ui::DictionaryRTSModelBase<pureTypeName>) {
            dict.set(word(pureTypeName::typeName + "Type"),
                     getRTSOptions<pureTypeName>());
        }
        for_each(type.members, [&](auto member) {
            if constexpr (refl::descriptor::is_field(member)) {
                using memberType = typename decltype(member)::value_type;
                auto name = ui::builder<pureTypeName>::demangle(
                  typeid(memberType).name());
                if (pureTypeName::debug > 2) {
                    Info << "is " << name << " default constructible? "
                         << std::is_default_constructible_v<memberType> << endl;
                    Info << "is " << name << " of pointer type? "
                         << ui::SomePtr<memberType> << endl;
                }
                if constexpr (ui::SomePtr<memberType>) {
                    // If a pointer; check if it is a pointer to a base RTS
                    // class
                    using elementType = typename memberType::element_type;
                    if constexpr (DictionaryRTSModelBase<elementType>) {
                        dictionary newDict = ui::builder<elementType>::schema();
                        dict.set(processKey(member.name.str()), newDict);
                    } else {
                        auto ftype = ui::builder<elementType>::demangle(
                          typeid(elementType).name());
                        word fname =
                          ui::builder<elementType>::familiarNaming(ftype);
                        string key = "__optional " + fname + " here__";
                        dict.set(processKey(member.name.str()), key);
                    }
                } else if constexpr (std::is_default_constructible_v<
                                       memberType>) {
                    dict.set(processKey(member.name.str()), memberType());
                } else {
                    word fname =
                      ui::builder<pureTypeName>::familiarNaming(name);
                    string key = "__value for " + fname + " here__";
                    dict.set(processKey(member.name.str()), key);
                }
            }
        });
        return dict;
    }

    /// @brief A function to build a dictionary from reflected class members and
    /// user input
    static dictionary schemaFromUser()
    {
        auto processKey = [](word key) {
            key.removeEnd('_');
            return key;
        };
        dictionary dict;
        using pureTypeName = std::remove_cvref_t<T>;
        constexpr auto type = refl::reflect<pureTypeName>();
        auto typeName =
          ui::builder<pureTypeName>::demangle(typeid(pureTypeName).name());
        if constexpr (ui::DictionaryRTSModelBase<pureTypeName>) {
            Info << "Choose a valid model from: "
                 << getRTSOptions<pureTypeName>() << endl;
            word validModel;
            std::getline(std::cin, validModel);
            dict.set(word(pureTypeName::typeName + "Type"), validModel);
        }
        for_each(type.members, [&](auto member) {
            if constexpr (refl::descriptor::is_field(member)) {
                using memberType = typename decltype(member)::value_type;
                auto memberTypeName = ui::builder<pureTypeName>::demangle(
                  typeid(memberType).name());
                if constexpr (ui::isUIMember<memberType>) {
                    using nestedType = typename memberType::value_type;
                    memberTypeName =
                      ui::builder<nestedType>::demangle(typeid(nestedType).name());
                }
                if (pureTypeName::debug > 2) {
                    Info << "is " << memberTypeName
                         << " default constructible? "
                         << std::is_default_constructible_v<memberType> << endl;
                    Info << "is " << memberTypeName << " of pointer type? "
                         << ui::SomePtr<memberType> << endl;
                }
                if constexpr (ui::SomePtr<memberType>) {
                    // If a pointer; check if it is a pointer to a base RTS
                    // class
                    using elementType = typename memberType::element_type;
                    if constexpr (DictionaryRTSModelBase<elementType>) {
                        // dictionary newDict =
                        // ui::builder<elementType>::schemaFromUser();
                        Info << "Choose a Model from"
                             << elementType::schemasPtr_->toc() << endl;
                        word modelType;
                        std::getline(std::cin, modelType);
                        dictionary newDict = elementType::schema(modelType);
                        dict.set(processKey(member.name.str()), newDict);
                    } else {
                        auto ftype = ui::builder<elementType>::demangle(
                          typeid(elementType).name());
                        word fname =
                          ui::builder<elementType>::familiarNaming(ftype);
                        // string key = "__optional " + fname + " here__";
                        Info << "Enter a value for " << typeName
                             << "::" << processKey(member.name.str()) << " (a "
                             << ftype << ", optional)" << endl;
                        word key;
                        std::getline(std::cin, key);
                        dict.set(processKey(member.name.str()), key);
                    }
                } /* else if constexpr (refl::descriptor::has_attribute<
                                       typename Foam::ui::defaulted<
                                         memberType>>(member)) {
                    word ftype =
                      ui::builder<memberType>::familiarNaming(memberTypeName);
                    if constexpr (ui::isUIMember<memberType>) {
                        ftype =
                          Foam::ui::builder<typename memberType::value_type>::
                            familiarNaming(memberType::v_type);
                    }
                    Info << "Enter a value for " << typeName
                         << "::" << processKey(member.name.str()) << " (a "
                         << ftype << " with default = "
                         << properties<memberType>::getDefault(member) << " )"
                         << endl;
                    word key;
                    std::getline(std::cin, key);
                    dict.set(processKey(member.name.str()), key);
                } else if constexpr (std::is_default_constructible_v<
                                       memberType>) {
                    word ftype =
                      ui::builder<memberType>::familiarNaming(memberTypeName);
                    Info << "*----------------- "
                         << (ui::isUIMember<memberType>) << endl;
                    if constexpr (ui::isUIMember<memberType>) {
                        ftype =
                          Foam::ui::builder<typename memberType::value_type>::
                            familiarNaming(memberType::value_type);
                        Info << "Enter a value for " << typeName
                             << "::" << processKey(member.name.str()) << " (a "
                             << ftype
                             << " with default value: " << memberType::init()
                             << ")" << endl;
                    } else {
                        Info << "Enter a value for " << typeName
                             << "::" << processKey(member.name.str()) << " (a "
                             << ftype
                             << " with default type value: " << memberType()
                             << ")" << endl;
                    }
                    word key;
                    std::getline(std::cin, key);
                    dict.set(processKey(member.name.str()), key);
                } */
                else if constexpr (ui::isUIMember<memberType>) {
                    word ftype =
                      Foam::ui::builder<typename memberType::value_type>::
                        familiarNaming(memberTypeName);
                    using vType = typename memberType::value_type;
                    if constexpr (ui::SomePtr<vType>)  {
                        Info << "Enter a value for " << typeName
                             << "::" << processKey(member.name.str()) << " (a "
                             << ftype
                             << " with default value: " << "__optional__ " 
                             << ")" << endl;
                    } else {
                        Info << "Enter a value for " << typeName
                             << "::" << processKey(member.name.str()) << " (a "
                             << ftype
                             << " with default value: " << memberType::init()
                             << ")" << endl;
                }
                } else {
                    word ftype =
                      ui::builder<pureTypeName>::familiarNaming(memberTypeName);
                    Info << "Enter a value for " << typeName
                         << "::" << processKey(member.name.str()) << " (a "
                         << ftype << " )" << endl;
                    word key;
                    std::getline(std::cin, key);
                    dict.set(processKey(member.name.str()), key);
                }
            }
        });
        return dict;
    }
};

} /// !namespace ui

} /// !namespace Foam

#endif
