#ifndef origChild1Model_H
#define origChild1Model_H

#include "origOpenFOAMModel.H"

namespace Foam {

/// @brief a child model mimiking original OpenFOAM child models
/// and keeping an optional subModel that participates in RTS
/// @output_section author:elwardi
class origChild1Model : public origOpenFOAMModel {
protected:
    /// Optional sub-model to prevent infinite recursion
    autoPtr<origOpenFOAMModel> subModel_;

    /// A scalar Coefficient
    scalar k_;

public:
    /// Runtime type name
    TypeName("origChild1Model");

    /// Construct from dictionary
    explicit origChild1Model(const dictionary& dict);

    /// Destruct origChild1Models
    virtual ~origChild1Model() = default;

    /// Update origChild1Model
    virtual autoPtr<origOpenFOAMModel> clone() const;

    /// Verify type name
    virtual word verifyType() const {
        return typeName;
    }

    /// Befriend typeinfo for this class
    friend class refl_impl::metadata::type_info__<origChild1Model>;
};

} // !namspace Foam

REFL_AUTO(type(Foam::origChild1Model, bases<Foam::origOpenFOAMModel>),
          field(subModel_),
          field(k_))

#endif

// ************************************************************************* //
