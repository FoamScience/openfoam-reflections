#ifndef reflectDictionary_H
#define reflectDictionary_H

#include <cstdlib>
#include <memory>
#include <string>
#include "HashTable.H"
#include "OStringStream.H"
#include "Ostream.H"
#include "dictionary.H"
#include "refl.H"
#include "reflectConcepts.H"
#include "uiElement.H"
#ifdef __GNUG__
#include <cxxabi.h>
#else
#pragma message( \
    "Warning: This compiler is not compatible with GCC ABI, mangled names will be displayed.")
#endif

namespace Foam {

/// @output_section author:elwardi
namespace Reflect {

struct _detail {
    /// @brief A function to return a familiar name from a C++ type name
    /// @return A human-readable string representing the type
    static word familiarNaming(word name) {
        HashTable<word> map;
        map.insert(
            "std::__cxx11::basic_string<char, std::char_traits<char>, "
            "std::allocator<char> >",
            "std::string");
        map.insert("long", "long integer");
        if (map.found(name))
            return map[name];
        return name;
    }

    /// @brief A function to demangle types from typeid.
    /// Only compiled for Gcc, other compilers will display mangled names
    /// @return demangled type name if compiled with GCC-compatible compilers,
    /// else, the mangled type name
    static word demangle(const char* name) {
#ifdef __GNUG__
        int status = -4;
        std::unique_ptr<char, void (*)(void*)> res{
            abi::__cxa_demangle(name, NULL, NULL, &status), std::free};
        return (status == 0) ? res.get() : name;
#else
        return name;
#endif
    }
};

/// @brief Utilities for generating skeleton dictionaries for classes
template <class T, bool Constructible = false>
struct reflect {
    /// @brief Get member value as string for construction purposes
    static void memberConstructibleValue(dictionary& skeleton, word key) {
        OStringStream oss;
        // Here UI members get max attention. Pointers to stuff will behave
        if constexpr (UIElement<T>) {
            if constexpr (SomePtr<typename T::valueType>) {
                auto def = T::init();
                if (def.valid()) {
                    oss << *T::init();
                }
            } else {
                oss << T::init();
            }
        } else {
            if constexpr (SomePtr<T>) {
                // if a pointer, override with underlying data
                using elementType = T::element_type;
                reflect<elementType, Constructible>::memberConstructibleValue(
                    skeleton, key);
                return;
            } else if constexpr (std::is_default_constructible_v<T>) {
                oss << T();
            }
        }
        if (oss.str().empty())
            oss << '"' << '"';
        if constexpr (Constructible) {
            skeleton.set<word>(key, oss.str());
        } else {
            skeleton.set<string>(key, oss.str());
        }
    }

    /// @brief Formats the list of registered RTS options  for a
    /// dictionary-based RTS model
    /// @return A string, listing available RTS options
    template <DictionaryRTSModelBase U>
    static word getRTSOptions() {
        word options = "\"<<RTSoption>> ( ";
        for (auto& e : U::dictionaryConstructorTablePtr_->toc()) {
            options += e + " ";
        }
        options += ")\"";
        return options;
    }

    /// @brief Get a list of registered RTS options
    /// @return a vector of RTS options as strings
    template <DictionaryRTSModelBase U>
    static std::vector<string> getRTSOptionsArray() {
        std::vector<string> opts{""};
        for (const auto& e : U::dictionaryConstructorTablePtr_->toc()) {
            opts.push_back(e);
        }
        return opts;
    }

    /// @brief A static method to add an entry for a single "member", concrete
    /// logic is handled by concept-based specialization of this method
    /// @return the passed-in skeleton dict with added entries for memebr of
    /// type T
    static dictionary& memberSchema(dictionary& skeleton,
                                    const word& memberKey,
                                    const dictionary& config) {
        // Default behavior only shows type with no tags
        word memberTypeName;
        if constexpr (UIElement<T>) {
            memberTypeName =
                _detail::demangle(typeid(typename T::valueType).name());
        } else {
            memberTypeName = _detail::demangle(typeid(T).name());
        }
        // using pureTypeName = std::remove_cvref_t<T>;
        word ftype = _detail::familiarNaming(memberTypeName);
        if constexpr (Constructible) {
            memberConstructibleValue(skeleton, memberKey);
            return skeleton;
        }
        dictionary memberDict;
        memberDict.set<string>("type", ftype);
        memberConstructibleValue(memberDict, "default");
        // Here UI members get max attention.
        if constexpr (UIElement<T>) {
            memberDict.set("description", T::descr());
            if constexpr (UIElementWithMin<T>) {
                if constexpr (Constructible) memberDict.set("min", T::min());
                if constexpr (!Constructible) memberDict.set<string>("min", Foam::name(T::min()));
            }
            if constexpr (UIElementWithMax<T>) {
                if constexpr (Constructible) memberDict.set("max", T::max());
                if constexpr (!Constructible) memberDict.set<string>("max", Foam::name(T::max()));
            }
        }
        skeleton.subDictOrAdd(memberKey) = memberDict;
        return skeleton;
    }

    /// @brief A function to build a dictionary/JSON from reflected class
    /// members
    /// @param config: A configuration dictionary to specify concrete models for
    static dictionary schema(const dictionary& config) {
        /// Format member names, which are assumed to end with an underscore
        auto processKey = [](word key) {
            key.removeEnd('_');
            return key;
        };

        /// the dictionary hosting the required entries
        dictionary skeleton;

        using pureTypeName = std::remove_cvref_t<T>;
        constexpr auto type = refl::reflect<pureTypeName>();
        auto typeName = _detail::demangle(typeid(pureTypeName).name());

        if constexpr (DictionaryRTSModelBase<pureTypeName>) {
            word typeKey(pureTypeName::typeName + "Type");
            // if it's an RTS base and a concrete type is requested in the
            // config, get schema for concrete type instead else, communicate
            // options
            if (config.found(typeKey)) {
                auto delegateType = config.get<word>(typeKey);
                if constexpr (Constructible)
                    delegateType += "Construct";
                skeleton = T::schemasPtr_->operator[](delegateType)(config);
            } else {
                if constexpr (Constructible) {
                    skeleton.set(typeKey, getRTSOptions<pureTypeName>());
                } else {
                    dictionary typeDict;
                    typeDict.set<string>("type", typeKey);
                    typeDict.set<string>("default", getRTSOptions<pureTypeName>());
                    skeleton.subDictOrAdd(typeKey) = typeDict;
                }
            }
        }

        /// TODO: maybe extract this bit into a templated function
        if constexpr (WrapperDictionaryRTSBase<pureTypeName>) {
            using wrappedType = pureTypeName::wrappedType;
            word typeKey(wrappedType::typeName + "Type");
            // if it's an RTS base and a concrete type is requested in the
            // config, get schema for concrete type instead else, communicate
            // options
            if (config.found(typeKey)) {
                auto delegateType = config.get<word>(typeKey);
                if constexpr (Constructible)
                    delegateType += "Construct";
                skeleton = T::schemasPtr_->operator[](delegateType)(config);
            } else {
                if constexpr (DictionaryRTSModelBase<wrappedType>)
                {
                    auto opts = getRTSOptions<wrappedType>();
                    if constexpr (Constructible) {
                        skeleton.set(typeKey, opts);
                    } else {
                        dictionary typeDict;
                        typeDict.set<string>("type", typeKey);
                        typeDict.set<string>("default", opts);
                        skeleton.subDictOrAdd(string(typeKey)) = typeDict;
                    }
                } else {
                    //TODO: what to do if wrappedType is a child here
                }
            }
        }

        // if it's a concrete model for an RTS type, set type to typename
        for_each(refl::util::reflect_types(type.bases), [&](auto bs) {
            using baseType = typename decltype(bs)::type;
            if constexpr (DictionaryRTSModelImpl<baseType, pureTypeName>) {
                if constexpr (Constructible) {
                    skeleton.set(word(baseType::typeName + "Type"),
                                 pureTypeName::typeName);
                } else {
                    dictionary typeDict;
                    typeDict.set<string>("type", word(baseType::typeName+"Type"));
                    typeDict.set<string>("default", pureTypeName::typeName);
                    skeleton.subDictOrAdd(baseType::typeName+"Type") = typeDict;
                }
            }
        });

        // loop over members
        for_each(type.members, [&](auto member) {
            if constexpr (refl::descriptor::is_field(member)) {
                // get the type of this field
                using memberType = typename decltype(member)::value_type;
                auto memberTypeName =
                    _detail::demangle(typeid(memberType).name());
                auto memberKey = processKey(member.name.str());
                reflect<memberType, Constructible>::memberSchema(
                    skeleton, memberKey, config);
            }
        });
        return skeleton;
    }
};

// @brief Specialization of member schema for pointer types
template <SomePtr T, bool Constructible>
struct reflect<T, Constructible> {
    static dictionary& memberSchema(dictionary& skeleton,
                                    const word& memberKey,
                                    const dictionary& config) {
        dictionary memberDict;
        // Peek into underlying type
        using elementType = T::element_type;
        // if underlying type is managed by dictionary-based RTS, handle nested
        // RTS models
        if constexpr (DictionaryRTSModel<elementType>) {
            dictionary newDict;
            // if <subModelType> is requested in config, get schema for the
            // concrete model otherwise, run unconfigured schema
            if (config.found(memberKey + "Type")) {
                word delegated = config.get<word>(memberKey + "Type");
                newDict.set("type", delegated);
                if constexpr (Constructible)
                    delegated += "Construct";
                dictionary newConfig = config;
                newConfig.set(word(elementType::typeName + "Type"), delegated);
                newDict =
                    elementType::schemasPtr_->operator[](delegated)(newConfig);
            } else {
                newDict = reflect<elementType>::schema(dictionary::null);
            }
            if constexpr (Constructible) {
                memberDict.set("default", newDict);
            } else {
                memberDict.subDictOrAdd("default") = newDict;
            }
        } else {
            auto ftype = _detail::demangle(typeid(elementType).name());
            word tname = _detail::familiarNaming(ftype);
            dictionary newDict;
            newDict.set("type", word("<<onDemand>> ") + tname);
            reflect<elementType, Constructible>::memberConstructibleValue(
                newDict, "default");
            memberDict.set("default", newDict);
        }
        if constexpr (Constructible) {
            // reflect<elementType,
            // Constructible>::memberConstructibleValue(skeleton, memberKey);
            if constexpr (DictionaryRTSModel<elementType>) {
                skeleton.subDictOrAdd(memberKey) =
                    memberDict.subDict("default");
            } else {
                reflect<elementType, Constructible>::memberConstructibleValue(
                    skeleton, memberKey);
            }
        } else {
            skeleton.subDictOrAdd(memberKey) = memberDict.subDict("default");
        }
        return skeleton;
    }
};

}  // namespace Reflect

}  // namespace Foam

#endif
