#ifndef basicReflectedModel_H
#define basicReflectedModel_H

#include "HashTable.H"
#include "autoPtr.H"
#include "dictionary.H"
#include "runTimeSelectionTables.H"
#include "reflectionSchemaTable.H"
#include "reflectDictionary.H"
#include "vector.H"
#include "OFstream.H"

namespace Foam {

/// @brief Base class mimiking a standard OpenFOAM model
/// while having minimal dependencies but default-constructing
/// most of its members
/// @output_section author:elwardi
class basicReflectedModel {
protected:
    /// Configuration dictionary
    const dictionary& dict_;

    /// optional model name
    autoPtr<word> name_;

    /// m coefficient
    label m_;

    /// vector
    vector dir_;

    /// HashTable of words
    HashTable<word> ht_;

    /// A not-default-constructible member
    OFstream ndc_;

public:
    /// Runtime type name
    TypeName("basicReflectedModel");

    /// @brief RunTime selection table for basicReflectedModels
    declareRunTimeSelectionTable(autoPtr,
        basicReflectedModel,
        dictionary,
        (const dictionary& dict),
        (dict));

    /// @brief Schema table for basicReflectedModels
    /// @param first argument is base class type
    /// @param second argument is the schamfunction name from Reflect::reflect<T> (schema)
    /// @param third argument is return type of the schema function (dictionary)
    declareSchemaTable(basicReflectedModel, schema, dictionary, (const dictionary&));

    /// Construct from dictionary
    explicit basicReflectedModel(const dictionary& dict);

    /// Delete move construct
    basicReflectedModel(basicReflectedModel&& other) = delete;

    /// Delete default copy construct
    basicReflectedModel(const basicReflectedModel& other) = delete;

    /// Destruct basicReflectedModels
    virtual ~basicReflectedModel() { }

    /// @returns A dynamic clone of this model
    virtual autoPtr<basicReflectedModel> clone() const = 0;

    /// @returns the typename
    virtual word verifyType() const = 0;

    /// @returns The concrete model from dictionary
    /// as a pointer to base type
    static autoPtr<basicReflectedModel> New(const dictionary& dict);

    /// @returns the configuration dictionary
    const dictionary& dict() const
    {
        return dict_;
    }

    /// @returns m
    label m() const
    {
        return m_;
    }

    /// Deleted move assignment
    basicReflectedModel& operator=(basicReflectedModel&& other) = delete;

    /// Deleted copy assignment
    basicReflectedModel& operator=(const basicReflectedModel& other) = delete;

    /// Befriend typeinfo struct from reflection  backend
    friend class refl_impl::metadata::type_info__<basicReflectedModel>;
};

} /// !namespace Foam

REFL_AUTO(type(Foam::basicReflectedModel),
          field(name_),
          field(m_),
          field(dir_),
          field(ht_),
          field(ndc_))

#endif
