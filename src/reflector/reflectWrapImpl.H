#ifndef reflectWrapImpl_H
#define reflectWrapImpl_H

#include "reflectWrapAbstract.H"

namespace Foam {

/// @output_section author:elwardi
namespace Reflect {

template <class WrappedType>
struct wrapImpl : public wrapAbstract<WrappedType> {

    /// delegate typename
    TypeName(WrappedType::typeName_());

    /// Construct from dictionary
    explicit wrapImpl(const dictionary& dict);

    /// Destruct wrapperModels
    virtual ~wrapImpl() {}

    /// Befriend typeinfo struct from reflection backend
    //friend class refl_impl::metadata::type_info__<WrappedType>;
    friend class refl_impl::metadata::type_info__<wrapImpl<WrappedType>>;
};

}  // namespace Reflect

}  // namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif
