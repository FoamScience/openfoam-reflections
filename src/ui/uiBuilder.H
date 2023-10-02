#ifndef uiBuilder_H
#define uiBuilder_H

#include "refl.H"
#include "uiConcepts.H"
#include "HashTable.H"
#include "dictionary.H"
#include <cstdlib>
#include <memory>
#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace Foam
{

namespace ui
{

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
        map.insert("std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >", "std::string");
        map.insert("long", "long integer");
        if (map.found(name)) return map[name];
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
            std::unique_ptr<char, void(*)(void*)> res {
                abi::__cxa_demangle(name, NULL, NULL, &status),
                std::free
            };
            return (status==0) ? res.get() : name ;
    #else
            return name;
    #endif 
    }
};

} /// !namespace ui

} /// !namespace Foam

#endif
