#include "anotherModel.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam
{

namespace mff
{

defineTypeNameAndDebug(anotherModel, 0);
addToRunTimeSelectionTable(baseModel, anotherModel, dictionary);

addToSchemaTable(baseModel, anotherModel);

anotherModel::anotherModel
(
    const dictionary& dict
)
:
    baseModel(dict),
    scalars_(),
    doUpdates_()
{
}

autoPtr<baseModel> anotherModel::clone() const
{
    return nullptr;
}

} /// !namespace mff

} /// !namespace Foam

