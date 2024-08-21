#ifndef origOpenFOAMModel_H
#define origOpenFOAMModel_H

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
class origOpenFOAMModel {
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
    TypeName("origOpenFOAMModel");

    /// @brief RunTime selection table for origOpenFOAMModels
    declareRunTimeSelectionTable(autoPtr,
        origOpenFOAMModel,
        dictionary,
        (const dictionary& dict),
        (dict));

    /// @brief Schema table for origOpenFOAMModels
    /// @param first argument is base class type
    /// @param second argument is the schamfunction name from Reflect::reflect<T> (schema)
    /// @param third argument is return type of the schema function (dictionary)
    declareSchemaTable(origOpenFOAMModel, schema, dictionary, (const dictionary&));

    /// Construct from dictionary
    explicit origOpenFOAMModel(const dictionary& dict);

    /// Delete move construct
    origOpenFOAMModel(origOpenFOAMModel&& other) = delete;

    /// Delete default copy construct
    origOpenFOAMModel(const origOpenFOAMModel& other) = delete;

    /// Destruct origOpenFOAMModels
    virtual ~origOpenFOAMModel() { }

    /// @returns A dynamic clone of this model
    virtual autoPtr<origOpenFOAMModel> clone() const = 0;

    /// @returns the typename
    virtual word verifyType() const = 0;

    /// @returns The concrete model from dictionary
    /// as a pointer to base type
    static autoPtr<origOpenFOAMModel> New(const dictionary& dict);

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
    origOpenFOAMModel& operator=(origOpenFOAMModel&& other) = delete;

    /// Deleted copy assignment
    origOpenFOAMModel& operator=(const origOpenFOAMModel& other) = delete;

    /// Befriend typeinfo struct from reflection  backend
    friend class refl_impl::metadata::type_info__<origOpenFOAMModel>;
};

} /// !namespace Foam

REFL_AUTO(type(Foam::origOpenFOAMModel),
          field(name_),
          field(m_),
          field(dir_),
          field(ht_),
          field(ndc_))

#endif
