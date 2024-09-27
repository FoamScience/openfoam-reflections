#include "addToRunTimeSelectionTable.H"
#include "child2BasicReflectedModel.H"

namespace Foam {

defineTypeNameAndDebug(child2BasicReflectedModel, 0);
addToRunTimeSelectionTable(basicReflectedModel, child2BasicReflectedModel, dictionary);
addToSchemaTable(basicReflectedModel, child2BasicReflectedModel);

child2BasicReflectedModel::child2BasicReflectedModel(
    const dictionary& dict)
    : basicReflectedModel(dict)
    , someDict_()
    , labels_(dict.lookupOrDefault<labelList>("labels", labelList(1, 10)))
{
}

autoPtr<basicReflectedModel> child2BasicReflectedModel::clone() const
{
    return nullptr;
}

} /// !namespace Foam
