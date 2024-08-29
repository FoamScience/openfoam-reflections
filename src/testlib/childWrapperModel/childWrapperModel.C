#include "childWrapperModel.H"
#include "dictionary.H"

namespace Foam {

template <class Type>
childWrapperModel<Type>::childWrapperModel(const dictionary& dict)
    :
    wrapperModel<Type>(dict),
    childRegularModel<Type>(dict)
{}

}  // namespace Foam
