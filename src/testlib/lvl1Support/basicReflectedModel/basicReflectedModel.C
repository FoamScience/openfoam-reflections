#include "basicReflectedModel.H"

namespace Foam {

defineTypeNameAndDebug(basicReflectedModel, 0);
defineRunTimeSelectionTable(basicReflectedModel, dictionary);
defineSchemaTable(basicReflectedModel);

basicReflectedModel::basicReflectedModel(
    const dictionary& dict)
    : dict_(dict)
    , name_ { dict.found("name") ? new word(dict.get<string>("name")) : nullptr }
    , m_(dict.lookupOrDefault<label>("m", 15))
    , dir_(dict.lookupOrDefault<vector>("dir", vector { 1, 0, 0 }))
    , ht_(dict.lookupOrDefault<HashTable<word>>("ht", HashTable<word>()))
    , ndc_("/tmp/eazuttgv")
{
}

autoPtr<basicReflectedModel>
basicReflectedModel::New(
    const dictionary& dict)
{
    const word modelType(dict.get<word>(typeName+"Type"));

    Info << "Selecting basicReflectedModel model " << modelType << endl;

    auto cstrIter = dictionaryConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found()) {
        FatalIOErrorInLookup(
            dict,
            "basicReflectedModelType",
            modelType,
            *dictionaryConstructorTablePtr_)
            << exit(FatalIOError);
    }

    return autoPtr<basicReflectedModel>(cstrIter()(dict));
}

} /// !namespace Foam
