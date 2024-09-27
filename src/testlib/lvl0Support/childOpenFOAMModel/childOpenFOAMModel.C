#include "childOpenFOAMModel.H"
#include "dictionary.H"

namespace Foam {

template <class T>
childOpenFOAMModel<T>::childOpenFOAMModel
(
    const dictionary& dict
)
    : OpenFOAMModel<T>(dict),
      list_(2, T::zero) {}


}  // namespace Foam
