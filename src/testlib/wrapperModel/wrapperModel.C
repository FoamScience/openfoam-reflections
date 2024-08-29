#include "wrapperModel.H"
#include "dictionary.H"

namespace Foam {

template <class Type>
wrapperModel<Type>::wrapperModel
(
    const dictionary& dict
)
    : regularModel<Type>(dict) {}

}  // namespace Foam
