#ifndef childFullReflectedModel_H
#define childFullReflectedModel_H

#include "fullReflectedModel.H"
#include "basicReflectedModel.H"

namespace Foam {

/// @brief a child model mimiking original OpenFOAM child models
/// and keeping an optional subModel that participates in RTS
/// @output_section author:elwardi
class childFullReflectedModel : public fullReflectedModel {
protected:

    /// submodel
    autoPtr<basicReflectedModel> subModel_;

public:
    /// Runtime type name
    TypeName("childFullReflectedModel");

    /// Construct from dictionary
    explicit childFullReflectedModel(const dictionary& dict);

    /// Destruct childFullReflectedModels
    virtual ~childFullReflectedModel() = default;

    /// Update childFullReflectedModel
    virtual autoPtr<fullReflectedModel> clone() const;

    /// Verify type name
    virtual word verifyType() const {
        return typeName;
    }

    /// Befriend typeinfo for this class
    friend class refl_impl::metadata::type_info__<childFullReflectedModel>;
};

} // !namspace Foam

REFL_AUTO(type(Foam::childFullReflectedModel, bases<Foam::fullReflectedModel>),
          field(subModel_))

#endif

// ************************************************************************* //
