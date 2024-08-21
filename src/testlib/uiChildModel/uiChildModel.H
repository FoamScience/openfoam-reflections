#ifndef uiChildModel_H
#define uiChildModel_H

#include "uiModel.H"
#include "origOpenFOAMModel.H"

namespace Foam {

/// @brief a child model mimiking original OpenFOAM child models
/// and keeping an optional subModel that participates in RTS
/// @output_section author:elwardi
class uiChildModel : public uiModel {
protected:

    /// submodel
    autoPtr<origOpenFOAMModel> subModel_;

public:
    /// Runtime type name
    TypeName("uiChildModel");

    /// Construct from dictionary
    explicit uiChildModel(const dictionary& dict);

    /// Destruct uiChildModels
    virtual ~uiChildModel() = default;

    /// Update uiChildModel
    virtual autoPtr<uiModel> clone() const;

    /// Verify type name
    virtual word verifyType() const {
        return typeName;
    }

    /// Befriend typeinfo for this class
    friend class refl_impl::metadata::type_info__<uiChildModel>;
};

} // !namspace Foam

REFL_AUTO(type(Foam::uiChildModel, bases<Foam::uiModel>),
          field(subModel_))

#endif

// ************************************************************************* //
