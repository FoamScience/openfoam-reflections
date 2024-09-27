#ifndef child2BasicReflectedModel_H
#define child2BasicReflectedModel_H

#include "basicReflectedModel.H"

namespace Foam {

/// @brief a child model mimiking original OpenFOAM child models
/// and keeping an optional subModel that participates in RTS
/// @output_section author:elwardi
class child2BasicReflectedModel : public basicReflectedModel {
protected:
    /// This member wil not be reflected
    dictionary someDict_;

    /// A list of labels, will be reflected
    labelList labels_;

public:
    /// Runtime type name
    TypeName("child2BasicReflectedModel");

    /// Construct from dictionary
    explicit child2BasicReflectedModel(const dictionary& dict);

    /// Destruct child2BasicReflectedModels
    virtual ~child2BasicReflectedModel() = default;

    /// Update child2BasicReflectedModel
    virtual autoPtr<basicReflectedModel> clone() const;

    /// Verify type name
    virtual word verifyType() const {
        return typeName;
    }

    /// Befriend typeinfo for this class
    friend class refl_impl::metadata::type_info__<child2BasicReflectedModel>;
};

} // !namspace Foam

REFL_AUTO(type(Foam::child2BasicReflectedModel, bases<Foam::basicReflectedModel>),
          field(labels_))

#endif

// ************************************************************************* //
