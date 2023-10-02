#ifndef baseModel_H
#define baseModel_H

#include "IOdictionary.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "refl.H"
#include "vector.H"

namespace Foam
{

namespace mff
{

/// @brief Base class for some stuff
/// @hyde-owner elwardi
class baseModel
{
protected:

    /// Configuration dictionary
    const dictionary& dict_;

    /// Label
    label m_;

    /// Vector
    vector vv_;

public:

    /// Runtime type name
    TypeName("baseModel");

    /// @brief RunTime selection table for baseModeles
    declareRunTimeSelectionTable
    (
        autoPtr,
        baseModel,
        dictionary,
        (
            const dictionary& dict
        ),
        (dict)
    );

    /// Construct from dictionary
    explicit baseModel
    (
        const dictionary& dict
    );

    /// Move construct
    baseModel
    (
        baseModel&& other
    ) = default;

    /// Delete default copy construct
    baseModel
    (
        const baseModel& other
    ) = delete;

    /// Destruct baseModels
    virtual ~baseModel();

    /// @return A dynamic clone of this
    virtual autoPtr<baseModel> clone() const = 0;

    /// @return The concrete `baseRTSTemplate` from dictionary
    /// as a pointer to base type
    static autoPtr<baseModel> New
    (
        const dictionary& dict
    );

    /// @return the underlying dictionary
    const dictionary& dict() const;

    /// Deleted move assignment
    baseModel& operator=(baseModel&& other) = delete;

    /// Deleted copy assignment
    baseModel& operator=(const baseModel& other) = delete;

    friend class refl_impl::metadata::type_info__<baseModel>;
};

} /// !namespace mff

} /// !namespace Foam

REFL_AUTO
(
    type(Foam::mff::baseModel),
    field(m_),
    field(vv_)
)

#endif
