
#include "regularModel.H"
#include "primitiveFields.H"
#include "HashTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Define the constructor function hash tables

defineTemplateRunTimeSelectionTable(regularModel<scalar>, dictionary);
defineTemplateTypeNameAndDebug(regularModel<scalar>, 0);
defineTemplateRunTimeSelectionTable(regularModel<vector>, dictionary);
defineTemplateTypeNameAndDebug(regularModel<vector>, 0);
defineTemplateRunTimeSelectionTable(regularModel<sphericalTensor>, dictionary);
defineTemplateTypeNameAndDebug(regularModel<sphericalTensor>, 0);
defineTemplateRunTimeSelectionTable(regularModel<symmTensor>, dictionary);
defineTemplateTypeNameAndDebug(regularModel<symmTensor>, 0);
defineTemplateRunTimeSelectionTable(regularModel<tensor>, dictionary);
defineTemplateTypeNameAndDebug(regularModel<tensor>, 0);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam
