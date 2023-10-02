#ifndef childModel_H
#define childModel_H

#include "baseModel.H"
#include "mathematicalConstants.H"

namespace Foam
{

namespace mff
{

/// @brief childModel description
/// hyde-owner elwardi
class childModel
:
    public baseModel
{
protected:

    /// Nothing here, so remove this section
    word type_;
 
public:

    /// Runtime type name
    TypeName("childModel");

    /// Construct from dictionary
    explicit childModel(const dictionary& dict);

    /// Destruct childModels
    virtual ~childModel() = default;

    /// Update childModel
    virtual autoPtr<baseModel> clone() const;

    friend class refl_impl::metadata::type_info__<childModel>;
};

} /// !namspace mff

} // !namspace Foam

REFL_AUTO
(
    type(Foam::mff::childModel, bases<Foam::mff::baseModel>),
    field(type_)
)

#endif

// ************************************************************************* //
