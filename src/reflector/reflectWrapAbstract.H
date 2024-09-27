#ifndef reflectWrapAbstract_H
#define reflectWrapAbstract_H

#include "reflectDictionary.H"
#include "reflectionSchemaTable.H"

namespace Foam {

/// @output_section author:elwardi
namespace Reflect {

template <class WrappedType>
struct wrapAbstract : public WrappedType {
    /// @brief The type to be wrapped for reflection
    using wrappedType = WrappedType;

    /// delegate typename
    TypeName(WrappedType::typeName_());

    /// Generic schema table
    declareSchemaTable(wrapAbstract, schema, dictionary, (const dictionary&));

    /// Construct from dictionary
    explicit wrapAbstract(const dictionary& dict);

    /// Destruct wrapperModels
    virtual ~wrapAbstract() {}

    /// Befriend typeinfo struct from reflection backend
    //friend class refl_impl::metadata::type_info__<WrappedType>;
    friend class refl_impl::metadata::type_info__<wrapAbstract<WrappedType>>;
};

}  // namespace Reflect

}  // namespace Foam

#define wrapReflectImplementedModel(Base, Child, Targ)                         \
                                                                               \
    namespace Foam                                                             \
    {                                                                          \
        Reflect::wrapAbstract<Base<Targ>>::addSchemaToTable                    \
            <Reflect::wrapImpl<Child<Targ>>>                                   \
            add##Child##Targ##SchemaToSchemaTable                              \
            ( Child<Targ>::typeName );                                         \
    }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif
