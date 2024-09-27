#include "addToRunTimeSelectionTable.H"
#include "child1BasicReflectedModel.H"

namespace Foam {

defineTypeNameAndDebug(child1BasicReflectedModel, 0);
addToRunTimeSelectionTable(basicReflectedModel, child1BasicReflectedModel, dictionary);
addToSchemaTable(basicReflectedModel, child1BasicReflectedModel);

child1BasicReflectedModel::child1BasicReflectedModel(
    const dictionary& dict)
    : basicReflectedModel(dict)
    , subModel_(nullptr)
    , k_(dict.lookupOrDefault<scalar>("k", 8.0))
{
}

autoPtr<basicReflectedModel> child1BasicReflectedModel::clone() const
{
    return nullptr;
}

} /// !namespace Foam
