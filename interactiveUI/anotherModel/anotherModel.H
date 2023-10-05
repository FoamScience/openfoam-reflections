#ifndef anotherModel_H
#define anotherModel_H

#include "baseModel.H"
#include "mathematicalConstants.H"
#include "Switch.H"

namespace Foam
{

namespace mff
{

/// @brief anotherModel description
/// hyde-owner elwardi
class anotherModel
:
    public baseModel
{
protected:

    List<scalar>  scalars_;
    Switch doUpdates_;
 
public:

    /// Runtime type name
    TypeName("anotherModel");

    /// Construct from dictionary
    explicit anotherModel(const dictionary& dict);

    /// Destruct anotherModels
    virtual ~anotherModel() = default;

    /// Update anotherModel
    virtual autoPtr<baseModel> clone() const;

    friend class refl_impl::metadata::type_info__<anotherModel>;
};

} /// !namspace mff

} // !namspace Foam

REFL_AUTO
(
    type(Foam::mff::anotherModel, bases<Foam::mff::baseModel>),
    field(scalars_),
    field(doUpdates_)
)

#endif

// ************************************************************************* //
