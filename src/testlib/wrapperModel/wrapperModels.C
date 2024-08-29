
#include "wrapperModel.H"
#include "primitiveFields.H"
#include "HashTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Define the constructor function hash tables

defineSchemaTableTemplated(wrapperModel, vector);
defineSchemaTableTemplated(wrapperModel, sphericalTensor);
defineSchemaTableTemplated(wrapperModel, symmTensor);
defineSchemaTableTemplated(wrapperModel, tensor);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam
