#include "OpenFOAMModel.H"
#include "dictionary.H"

namespace Foam {

template <class T>
OpenFOAMModel<T>::OpenFOAMModel
(
    const dictionary& dict
)
    : dict_(dict),
      name_{dict.found("name") ? new word(dict.get<string>("name")) : nullptr},
      m_(dict.lookupOrDefault<T>("m", T::one)),
      dir_(dict.lookupOrDefault<vector>("dir", vector{1, 0, 0})),
      ht_(dict.lookupOrDefault<HashTable<word>>("ht", HashTable<word>())),
      ndc_("/tmp/eazuttgv") {}

template<class T>
autoPtr<OpenFOAMModel<T>> OpenFOAMModel<T>::New(const dictionary& dict) {
    const word modelType(dict.get<word>(typeName + "Type"));

    Info << "Selecting OpenFOAMModel model " << modelType << endl;

    auto cstrIter = dictionaryConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found()) {
        FatalIOErrorInLookup(dict, "OpenFOAMModelType", modelType,
                             *dictionaryConstructorTablePtr_)
            << exit(FatalIOError);
    }

    return autoPtr<OpenFOAMModel<T>>(cstrIter()(dict));
}

}  // namespace Foam
