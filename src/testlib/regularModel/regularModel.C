#include "regularModel.H"
#include "dictionary.H"

namespace Foam {

template <class T>
regularModel<T>::regularModel
(
    const dictionary& dict
)
    : dict_(dict),
      name_{dict.found("name") ? new word(dict.get<string>("name")) : nullptr},
      m_(dict.lookupOrDefault<T>("m", T::zero)),
      dir_(dict.lookupOrDefault<vector>("dir", vector{1, 0, 0})),
      ht_(dict.lookupOrDefault<HashTable<word>>("ht", HashTable<word>())),
      ndc_("/tmp/eazuttgv") {}

template<class T>
autoPtr<regularModel<T>> regularModel<T>::New(const dictionary& dict) {
    const word modelType(dict.get<word>(typeName + "Type"));

    Info << "Selecting regularModel model " << modelType << endl;

    auto cstrIter = dictionaryConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found()) {
        FatalIOErrorInLookup(dict, "regularModelType", modelType,
                             *dictionaryConstructorTablePtr_)
            << exit(FatalIOError);
    }

    return autoPtr<regularModel<T>>(cstrIter()(dict));
}

}  // namespace Foam
