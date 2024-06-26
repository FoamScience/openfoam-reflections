#include "childModel.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam
{

namespace mff
{

defineTypeNameAndDebug(childModel, 0);
addToRunTimeSelectionTable(baseModel, childModel, dictionary);

addToSchemaTable(baseModel, childModel);

childModel::childModel
(
    const dictionary& dict
)
:
    baseModel(dict),
    type_(),
    vector_(),
    subModel_()
{
}

autoPtr<baseModel> childModel::clone() const
{
    return nullptr;
}

} /// !namespace mff

} /// !namespace Foam

