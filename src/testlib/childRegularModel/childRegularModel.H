#ifndef childRegularModel_H
#define childRegularModel_H

#include "regularModel.H"

namespace Foam {

/// @brief Base class mimiking a standard OpenFOAM model
/// with no reflection support what's so ever
/// @output_section author:elwardi
template<class T>
class childRegularModel : public regularModel<T> {
protected:
    // list of T items
    List<T> list_;

public:
    /// Runtime type name
    TypeName("childRegularModel");

    /// Construct from dictionary
    explicit childRegularModel(const dictionary& dict);

    /// Delete move construct
    childRegularModel(childRegularModel&& other) = delete;

    /// Delete default copy construct
    childRegularModel(const childRegularModel& other) = delete;

    /// Destruct childRegularModels
    virtual ~childRegularModel() { }

    /// @returns A dynamic clone of this model
    virtual autoPtr<regularModel<T>> clone() const { return nullptr; }

    /// @returns the typename
    virtual word verifyType() const {
        return typeName_();
    }

    /// Deleted move assignment
    childRegularModel& operator=(childRegularModel&& other) = delete;

    /// Deleted copy assignment
    childRegularModel& operator=(const childRegularModel& other) = delete;
};

} /// !namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "childRegularModel.C"
#endif
#endif
