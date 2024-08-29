#ifndef wrapperModel_H
#define wrapperModel_H

#include "regularModel.H"
#include "reflectionSchemaTable.H"
#include "reflectDictionary.H"

namespace Foam {

/// @brief Base class mimiking a standard OpenFOAM model
/// with no reflection support what's so ever
/// @output_section author:elwardi
template<class Type>
struct wrapperModel
: public regularModel<Type> {

    /// @brief The type to be wrapped for reflection
    using wrappedType = regularModel<Type>;

    TypeName("regularModel");

    /// @brief Schema table for origOpenFOAMModels
    /// @param first argument is base class type
    /// @param second argument is the schema function name from Reflect::reflect<T> (schema)
    /// @param third argument is return type of the schema function (dictionary)
    declareSchemaTable(wrapperModel, schema, dictionary, (const dictionary&));

    /// Construct from dictionary
    explicit wrapperModel(const dictionary& dict);

    /// Destruct wrapperModels
    virtual ~wrapperModel() { }

    /// Befriend typeinfo struct from reflection  backend
    friend class refl_impl::metadata::type_info__<regularModel<Type>>;
    friend class refl_impl::metadata::type_info__<wrapperModel<Type>>;
};

} /// !namespace Foam

REFL_AUTO(template((typename Type), (Foam::wrapperModel<Type>)),
        field(name_),
        field(m_),
        field(dir_),
        field(ht_),
        field(ndc_))

#define defineSchemaTableTemplated(baseType, Targ)                             \
    template<> baseType<Targ>::schemaTable                                     \
        *baseType<Targ>::schemasPtr_(nullptr);                                 \
    template<> void baseType<Targ>::constructSchemaTables() {                  \
        if (baseType<Targ>::schemasPtr_ == nullptr) {                          \
            baseType<Targ>::schemasPtr_ = new baseType<Targ>::schemaTable;     \
        }                                                                      \
    }                                                                          \
    template<> void baseType<Targ>::destroySchemaTables() {                    \
        if (baseType<Targ>::schemasPtr_) {                                     \
            delete baseType<Targ>::schemasPtr_;                                \
            baseType<Targ>::schemasPtr_ = nullptr;                             \
        }                                                                      \
    }

#define makeWrapperModelSchema(SS, Type)                                       \
                                                                               \
    namespace Foam                                                             \
    {                                                                          \
        wrapperModel<Type>::addSchemaToTable<wrapperModel<Type>>               \
            add##SS##Type##SchemaToSchemaTable                                 \
            ( SS<Type>::wrappedType::typeName );                               \
    }

#define makeWrapperModelSchemas(SS)                                            \
                                                                               \
makeWrapperModelSchema(SS, vector)                                             \
makeWrapperModelSchema(SS, sphericalTensor)                                    \
makeWrapperModelSchema(SS, symmTensor)                                         \
makeWrapperModelSchema(SS, tensor)


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "wrapperModel.C"
#endif
#endif
