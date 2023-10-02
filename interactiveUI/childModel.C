#include "childModel.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam
{

namespace mff
{

defineTypeNameAndDebug(childModel, 0);
addToRunTimeSelectionTable(baseModel, childModel, dictionary);

childModel::childModel
(
    const dictionary& dict
)
:
    baseModel(dict),
    type_()
{
}

autoPtr<baseModel> childModel::clone() const
{
    return nullptr;
}

} /// !namespace mff

} /// !namespace Foam

