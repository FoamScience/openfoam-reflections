#include "addToRunTimeSelectionTable.H"
#include "origChild2Model.H"

namespace Foam {

defineTypeNameAndDebug(origChild2Model, 0);
addToRunTimeSelectionTable(origOpenFOAMModel, origChild2Model, dictionary);
addToSchemaTable(origOpenFOAMModel, origChild2Model);

origChild2Model::origChild2Model(
    const dictionary& dict)
    : origOpenFOAMModel(dict)
    , someDict_()
    , labels_(dict.lookupOrDefault<labelList>("labels", labelList(1, 10)))
{
}

autoPtr<origOpenFOAMModel> origChild2Model::clone() const
{
    return nullptr;
}

} /// !namespace Foam
