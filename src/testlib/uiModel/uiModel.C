#include "uiModel.H"

namespace Foam {

using Reflect::uiElement;

defineTypeNameAndDebug(uiModel, 0);
defineRunTimeSelectionTable(uiModel, dictionary);
defineSchemaTable(uiModel);
addToSchemaTable(uiModel, uiModel);

uiModel::uiModel(
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

autoPtr<uiModel>
uiModel::New(
    const dictionary& dict)
{
    const word modelType(dict.get<word>(typeName + "Type"));

    Info << "Selecting uiModel model " << modelType << endl;

    auto cstrIter = dictionaryConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found()) {
        FatalIOErrorInLookup(
            dict,
            "uiModelType",
            modelType,
            *dictionaryConstructorTablePtr_)
            << exit(FatalIOError);
    }

    return autoPtr<uiModel>(cstrIter()(dict));
}

} /// !namespace Foam
