#include "addToRunTimeSelectionTable.H"
#include "origChild1Model.H"

namespace Foam {

defineTypeNameAndDebug(origChild1Model, 0);
addToRunTimeSelectionTable(origOpenFOAMModel, origChild1Model, dictionary);
addToSchemaTable(origOpenFOAMModel, origChild1Model);

origChild1Model::origChild1Model(
    const dictionary& dict)
    : origOpenFOAMModel(dict)
    , subModel_(nullptr)
    , k_(dict.lookupOrDefault<scalar>("k", 8.0))
{
}

autoPtr<origOpenFOAMModel> origChild1Model::clone() const
{
    return nullptr;
}

} /// !namespace Foam
