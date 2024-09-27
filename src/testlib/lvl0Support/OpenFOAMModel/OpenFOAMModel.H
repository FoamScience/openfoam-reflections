#ifndef OpenFOAMModel_H
#define OpenFOAMModel_H

#include "HashTable.H"
#include "autoPtr.H"
#include "dictionary.H"
#include "runTimeSelectionTables.H"
#include "vector.H"
#include "tensor.H"
#include "OFstream.H"

namespace Foam {

/// @brief Base class mimiking a standard OpenFOAM model
/// with no reflection support what's so ever
/// @output_section author:elwardi
template<class T>
class OpenFOAMModel {
protected:
    /// Configuration dictionary
    const dictionary& dict_;

    /// optional model name
    autoPtr<word> name_;

    /// m coefficient
    T m_;

    /// vector
    vector dir_;

    /// HashTable of words
    HashTable<word> ht_;

    /// A not-default-constructible member
    OFstream ndc_;

public:
    /// Runtime type name
    TypeName("OpenFOAMModel");

    /// @brief RunTime selection table for OpenFOAMModels
    declareRunTimeSelectionTable(autoPtr,
        OpenFOAMModel,
        dictionary,
        (const dictionary& dict),
        (dict));

    /// Construct from dictionary
    explicit OpenFOAMModel(const dictionary& dict);

    /// Delete move construct
    OpenFOAMModel(OpenFOAMModel&& other) = delete;

    /// Delete default copy construct
    OpenFOAMModel(const OpenFOAMModel& other) = delete;

    /// Destruct OpenFOAMModels
    virtual ~OpenFOAMModel() { }

    /// @returns A dynamic clone of this model
    virtual autoPtr<OpenFOAMModel> clone() const = 0;

    /// @returns the typename
    virtual word verifyType() const = 0;

    /// @returns The concrete model from dictionary
    /// as a pointer to base type
    static autoPtr<OpenFOAMModel> New(const dictionary& dict);

    /// @returns the configuration dictionary
    const dictionary& dict() const
    {
        return dict_;
    }

    /// @returns m
    T m() const
    {
        return m_;
    }

    /// Deleted move assignment
    OpenFOAMModel& operator=(OpenFOAMModel&& other) = delete;

    /// Deleted copy assignment
    OpenFOAMModel& operator=(const OpenFOAMModel& other) = delete;
};

} /// !namespace Foam

#define makeRegularModel(SS, Type)                                             \
                                                                               \
    defineTemplateTypeNameAndDebug(Foam::SS<Foam::Type>, 0);                   \
    namespace Foam                                                             \
    {                                                                          \
        OpenFOAMModel<Type>::adddictionaryConstructorToTable<SS<Type>>          \
            add##SS##Type##dictionaryConstructorToTable_;                      \
    }

#define makeRegularModels(SS)                                                  \
                                                                               \
makeRegularModel(SS, vector)                                                   \
makeRegularModel(SS, sphericalTensor)                                          \
makeRegularModel(SS, symmTensor)                                               \
makeRegularModel(SS, tensor)


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "OpenFOAMModel.C"
#endif
#endif
