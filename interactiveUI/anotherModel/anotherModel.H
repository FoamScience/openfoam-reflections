#ifndef anotherModel_H
#define anotherModel_H

#include "Switch.H"
#include "baseModel.H"

namespace Foam {

namespace mff {

/// @brief anotherModel description
/// hyde-owner elwardi
class anotherModel : public baseModel
{
  protected:
    ui::member<List<scalar>,
               [] { return List<scalar>{ 5, 2 }; },
               [] { return "scalars_ description"; }>
      scalars_;
    ui::member<Switch,
               [] { return true; },
               [] { return "doUpdates_ description"; }>
      doUpdates_;
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

REFL_AUTO(type(Foam::mff::anotherModel, bases<Foam::mff::baseModel>),
          field(scalars_),
          field(doUpdates_))

#endif

// ************************************************************************* //
