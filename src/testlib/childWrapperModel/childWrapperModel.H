#ifndef childWrapperModel_H
#define childWrapperModel_H

#include "wrapperModel.H"
#include "childRegularModel.H"

namespace Foam {

/// @brief Base class mimiking a standard OpenFOAM model
/// with no reflection support what's so ever
/// @output_section author:elwardi
template<class Type>
struct childWrapperModel
  : public wrapperModel<Type>,
    public childRegularModel<Type> {

    /// @brief The type to be wrapped for reflection
    using wrappedType = childRegularModel<Type>;

    /// Construct from dictionary
    explicit childWrapperModel(const dictionary& dict);

    /// Destruct childWrapperModels
    virtual ~childWrapperModel() { }

    /// Befriend typeinfo struct from reflection  backend
    friend class refl_impl::metadata::type_info__<childWrapperModel<Type>>;
};

} /// !namespace Foam

REFL_AUTO(template((typename Type), (Foam::childWrapperModel<Type>), bases<Foam::wrapperModel<Type>>),
          field(list_))
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "childWrapperModel.C"
#endif
#endif
