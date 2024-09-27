#include "fullReflectedModel.H"

namespace Foam {

using Reflect::uiElement;

defineTypeNameAndDebug(fullReflectedModel, 0);
defineRunTimeSelectionTable(fullReflectedModel, dictionary);
defineSchemaTable(fullReflectedModel);

fullReflectedModel::fullReflectedModel(
    const dictionary& dict)
    : dict_(dict)
    , name_(
          withDefaultCaptuting(autoPtr<word>(new word(dict.get<string>("name"))), dict),
          withDefaultCaptuting(dict.found("name"), dict))
    , m_(dict, "m")
    , dir_(dict, "dir")
    , ht_(dict, "ht")
    , ndc_("/tmp/eazuttgv")
{
}

autoPtr<fullReflectedModel>
fullReflectedModel::New(
    const dictionary& dict)
{
    const word modelType(dict.get<word>(typeName + "Type"));

    Info << "Selecting fullReflectedModel model " << modelType << endl;

    auto cstrIter = dictionaryConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found()) {
        FatalIOErrorInLookup(
            dict,
            "fullReflectedModelType",
            modelType,
            *dictionaryConstructorTablePtr_)
            << exit(FatalIOError);
    }

    return autoPtr<fullReflectedModel>(cstrIter()(dict));
}

} /// !namespace Foam
