#include "addToRunTimeSelectionTable.H"
#include "uiChildModel.H"

namespace Foam {

defineTypeNameAndDebug(uiChildModel, 0);
addToRunTimeSelectionTable(uiModel, uiChildModel, dictionary);
addToSchemaTable(uiModel, uiChildModel);

uiChildModel::uiChildModel(
    const dictionary& dict)
    : uiModel(dict)
    , subModel_(nullptr)
{
}

autoPtr<uiModel> uiChildModel::clone() const
{
    return nullptr;
}

} /// !namespace Foam
