#ifndef uiModel_H
#define uiModel_H

#include "HashTable.H"
#include "autoPtr.H"
#include "dictionary.H"
#include "runTimeSelectionTables.H"
#include "reflectionSchemaTable.H"
#include "reflectDictionary.H"
#include "uiElement.H"
#include "vector.H"
#include "OFstream.H"

namespace Foam {

using Reflect::uiElement;

/// @brief Base class taking advantage of uiElements
/// @output_section author:elwardi
class uiModel {
protected:
    /// Configuration dictionary
    dictionary dict_;

    /// optional model name
    uiElement<autoPtr<word>,
        withDefault(nullptr),
        withDescription("name of uiModel")
    > name_;

    /// m coefficient
    uiElement<label,
        withDefault(15),
        withDescription("m coefficient"),
        withMin(10),
        withMax(20)
    > m_;

    /// vector
    uiElement<vector,
        withDefault((vector {1,0,0})),
        withDescription("model direction")
    > dir_;

    /// HashTable of words, default set through a custom Lambda
    uiElement<HashTable<word>,
        []() {
            HashTable<word> ht;
            ht.set("key1", "value1");
            ht.set("key2", "value2");
            return ht;
        },
        withDescription("model hash table of words")
    > ht_;

    /// A not-default-constructible member so not managed with uiElement
    OFstream ndc_;

public:
    /// Runtime type name
    TypeName("uiModel");

    /// @brief RunTime selection table for uiModels
    declareRunTimeSelectionTable(autoPtr,
        uiModel,
        dictionary,
        (const dictionary& dict),
        (dict));

    /// @brief Schema table for uiModels
    /// @param first argument is base class type
    /// @param second argument is the schamfunction name from Reflect::reflect<T> (schema)
    /// @param third argument is return type of the schema function (dictionary)
    declareSchemaTable(uiModel, schema, dictionary, (const dictionary&));

    /// Construct from dictionary
    explicit uiModel(const dictionary& dict);

    /// Delete move construct
    uiModel(uiModel&& other) = delete;

    /// Delete default copy construct
    uiModel(const uiModel& other) = delete;

    /// Destruct uiModels
    virtual ~uiModel() { }

    /// @returns A dynamic clone of this model
    virtual autoPtr<uiModel> clone() const = 0;

    /// @returns the typename
    virtual word verifyType() const = 0;

    /// @returns The concrete model from dictionary
    /// as a pointer to base type
    static autoPtr<uiModel> New(const dictionary& dict);

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

    /// @returns ht
    HashTable<word> ht() const
    {
        return ht_;
    }

    /// Deleted move assignment
    uiModel& operator=(uiModel&& other) = delete;

    /// Deleted copy assignment
    uiModel& operator=(const uiModel& other) = delete;

    /// Befriend typeinfo struct from reflection  backend
    friend class refl_impl::metadata::type_info__<uiModel>;
};

} /// !namespace Foam

REFL_AUTO(type(Foam::uiModel),
          field(name_),
          field(m_),
          field(dir_),
          field(ht_),
          field(ndc_))

#endif
