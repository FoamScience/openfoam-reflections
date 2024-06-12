#ifndef childModel_H
#define childModel_H

#include "baseModel.H"
#include "uiMember.H"

namespace Foam {

namespace mff {

/// @brief childModel description
/// hyde-owner elwardi
class childModel : public baseModel
{
  protected:
    /// Nothing here, so remove this section
    ui::
      member<word, [] { return "default"; }, [] { return "type_ desctiption"; }>
        type_;
    ui::member<vector,
               [] { return vector{ 15, 50, 80 }; },
               [] { return "vector_ desctiption"; }>
      vector_;
    ui::member<autoPtr<baseModel>,
               [] { return nullptr; },
               [] { return "subModel_ desctiption"; }>
      subModel_;

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

REFL_AUTO(type(Foam::mff::childModel, bases<Foam::mff::baseModel>),
          field(type_),
          field(vector_),
          field(subModel_))

#endif

// ************************************************************************* //
