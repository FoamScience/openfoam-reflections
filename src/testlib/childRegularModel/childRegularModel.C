#include "childRegularModel.H"
#include "dictionary.H"

namespace Foam {

template <class T>
childRegularModel<T>::childRegularModel
(
    const dictionary& dict
)
    : regularModel<T>(dict),
      list_(2, T::zero) {}


}  // namespace Foam
