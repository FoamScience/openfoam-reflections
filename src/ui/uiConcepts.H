#ifndef uiConcepts_H
#define uiConcepts_H

#include "refl.H"
#include "word.H"
#include "autoPtr.H"

namespace Foam
{

namespace ui
{

/// @brief A concept for OpenFOAM classes with RTS capabilities which get constructed by looking at
/// a dictionary.
/// @warning Not yet accurate
/// @hyde-owner elwardi
template<class T>
concept DictionaryRTSModel = requires {
    { T::typeName } -> std::convertible_to<word>;
    { T::dictionaryConstructorTablePtr_ }; // -> T::dictionaryConstructorTableType*
    //{ &T::New };
};

} /// !namespace ui

} /// !namespace Foam

#endif
