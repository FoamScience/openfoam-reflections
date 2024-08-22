#include "origOpenFOAMModel.H"

namespace Foam {

defineTypeNameAndDebug(origOpenFOAMModel, 0);
defineRunTimeSelectionTable(origOpenFOAMModel, dictionary);
defineSchemaTable(origOpenFOAMModel);

origOpenFOAMModel::origOpenFOAMModel(
    const dictionary& dict)
    : dict_(dict)
    , name_ { dict.found("name") ? new word(dict.get<string>("name")) : nullptr }
    , m_(dict.lookupOrDefault<label>("m", 15))
    , dir_(dict.lookupOrDefault<vector>("dir", vector { 1, 0, 0 }))
    , ht_(dict.lookupOrDefault<HashTable<word>>("ht", HashTable<word>()))
    , ndc_("/tmp/eazuttgv")
{
}

autoPtr<origOpenFOAMModel>
origOpenFOAMModel::New(
    const dictionary& dict)
{
    const word modelType(dict.get<word>(typeName+"Type"));

    Info << "Selecting origOpenFOAMModel model " << modelType << endl;

    auto cstrIter = dictionaryConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found()) {
        FatalIOErrorInLookup(
            dict,
            "origOpenFOAMModelType",
            modelType,
            *dictionaryConstructorTablePtr_)
            << exit(FatalIOError);
    }

    return autoPtr<origOpenFOAMModel>(cstrIter()(dict));
}

} /// !namespace Foam
