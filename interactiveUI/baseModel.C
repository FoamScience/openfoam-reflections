#include "baseModel.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam {

namespace mff {

defineTypeNameAndDebug(baseModel, 0);
defineRunTimeSelectionTable(baseModel, dictionary);

baseModel::baseModel
(
    const dictionary& dict
)
:
    dict_(dict),
    m_(),
    vv_()
{}

baseModel::~baseModel()
{}

const dictionary& baseModel::dict() const
{
    return dict_;
}

autoPtr<baseModel>
baseModel::New
(
    const dictionary& dict
)
{
    const word modelType(dict.get<word>("baseModelType"));

    Info<< "Selecting baseModel model " << modelType << endl;

    auto cstrIter = dictionaryConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found())
    {
        FatalIOErrorInLookup
        (
            dict,
            "baseModelType",
            modelType,
            *dictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    return autoPtr<baseModel>(cstrIter()(dict));
}

} /// !namespace mff

} /// !namespace Foam

