#ifndef child1BasicReflectedModel_H
#define child1BasicReflectedModel_H

#include "basicReflectedModel.H"

namespace Foam {

/// @brief a child model mimiking original OpenFOAM child models
/// and keeping an optional subModel that participates in RTS
/// @output_section author:elwardi
class child1BasicReflectedModel : public basicReflectedModel {
protected:
    /// Optional sub-model to prevent infinite recursion
    autoPtr<basicReflectedModel> subModel_;

    /// A scalar Coefficient
    scalar k_;

public:
    /// Runtime type name
    TypeName("child1BasicReflectedModel");

    /// Construct from dictionary
    explicit child1BasicReflectedModel(const dictionary& dict);

    /// Destruct child1BasicReflectedModels
    virtual ~child1BasicReflectedModel() = default;

    /// Update child1BasicReflectedModel
    virtual autoPtr<basicReflectedModel> clone() const;

    /// Verify type name
    virtual word verifyType() const {
        return typeName;
    }

    /// Befriend typeinfo for this class
    friend class refl_impl::metadata::type_info__<child1BasicReflectedModel>;
};

} // !namspace Foam

REFL_AUTO(type(Foam::child1BasicReflectedModel, bases<Foam::basicReflectedModel>),
          field(subModel_),
          field(k_))

#endif

// ************************************************************************* //
