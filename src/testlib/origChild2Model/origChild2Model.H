#ifndef origChild2Model_H
#define origChild2Model_H

#include "origOpenFOAMModel.H"

namespace Foam {

/// @brief a child model mimiking original OpenFOAM child models
/// and keeping an optional subModel that participates in RTS
/// @output_section author:elwardi
class origChild2Model : public origOpenFOAMModel {
protected:
    /// This member wil not be reflected
    dictionary someDict_;

    /// A list of labels, will be reflected
    labelList labels_;

public:
    /// Runtime type name
    TypeName("origChild2Model");

    /// Construct from dictionary
    explicit origChild2Model(const dictionary& dict);

    /// Destruct origChild2Models
    virtual ~origChild2Model() = default;

    /// Update origChild2Model
    virtual autoPtr<origOpenFOAMModel> clone() const;

    /// Verify type name
    virtual word verifyType() const {
        return typeName;
    }

    /// Befriend typeinfo for this class
    friend class refl_impl::metadata::type_info__<origChild2Model>;
};

} // !namspace Foam

REFL_AUTO(type(Foam::origChild2Model, bases<Foam::origOpenFOAMModel>),
          field(labels_))

#endif

// ************************************************************************* //
