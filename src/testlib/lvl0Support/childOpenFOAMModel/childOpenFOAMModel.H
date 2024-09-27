#ifndef childOpenFOAMModel_H
#define childOpenFOAMModel_H

#include "OpenFOAMModel.H"

namespace Foam {

/// @brief Base class mimiking a standard OpenFOAM model
/// with no reflection support what's so ever
/// @output_section author:elwardi
template<class T>
class childOpenFOAMModel : public OpenFOAMModel<T> {
protected:
    // list of T items
    List<T> list_;

public:
    /// Runtime type name
    TypeName("childOpenFOAMModel");

    /// Construct from dictionary
    explicit childOpenFOAMModel(const dictionary& dict);

    /// Delete move construct
    childOpenFOAMModel(childOpenFOAMModel&& other) = delete;

    /// Delete default copy construct
    childOpenFOAMModel(const childOpenFOAMModel& other) = delete;

    /// Destruct childOpenFOAMModels
    virtual ~childOpenFOAMModel() { }

    /// @returns A dynamic clone of this model
    virtual autoPtr<OpenFOAMModel<T>> clone() const { return nullptr; }

    /// @returns the typename
    virtual word verifyType() const {
        return typeName_();
    }

    /// Deleted move assignment
    childOpenFOAMModel& operator=(childOpenFOAMModel&& other) = delete;

    /// Deleted copy assignment
    childOpenFOAMModel& operator=(const childOpenFOAMModel& other) = delete;
};

} /// !namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "childOpenFOAMModel.C"
#endif
#endif
