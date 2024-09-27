
#include "OpenFOAMModel.H"
#include "primitiveFields.H"
#include "HashTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Define the constructor function hash tables

defineTemplateRunTimeSelectionTable(OpenFOAMModel<scalar>, dictionary);
defineTemplateTypeNameAndDebug(OpenFOAMModel<scalar>, 0);
defineTemplateRunTimeSelectionTable(OpenFOAMModel<vector>, dictionary);
defineTemplateTypeNameAndDebug(OpenFOAMModel<vector>, 0);
defineTemplateRunTimeSelectionTable(OpenFOAMModel<sphericalTensor>, dictionary);
defineTemplateTypeNameAndDebug(OpenFOAMModel<sphericalTensor>, 0);
defineTemplateRunTimeSelectionTable(OpenFOAMModel<symmTensor>, dictionary);
defineTemplateTypeNameAndDebug(OpenFOAMModel<symmTensor>, 0);
defineTemplateRunTimeSelectionTable(OpenFOAMModel<tensor>, dictionary);
defineTemplateTypeNameAndDebug(OpenFOAMModel<tensor>, 0);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam
