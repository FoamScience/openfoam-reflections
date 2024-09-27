#include "addToRunTimeSelectionTable.H"
#include "childFullReflectedModel.H"

namespace Foam {

defineTypeNameAndDebug(childFullReflectedModel, 0);
addToRunTimeSelectionTable(fullReflectedModel, childFullReflectedModel, dictionary);
addToSchemaTable(fullReflectedModel, childFullReflectedModel);

childFullReflectedModel::childFullReflectedModel(
    const dictionary& dict)
    : fullReflectedModel(dict)
    , subModel_(nullptr)
{
}

autoPtr<fullReflectedModel> childFullReflectedModel::clone() const
{
    return nullptr;
}

} /// !namespace Foam
