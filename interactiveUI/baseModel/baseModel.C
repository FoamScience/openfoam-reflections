#include "baseModel.H"
#include "addToRunTimeSelectionTable.H"


namespace Foam {

namespace mff {

defineTypeNameAndDebug(baseModel, 0);
defineRunTimeSelectionTable(baseModel, dictionary);

defineSchemaTable(baseModel);

void baseModel::constructSchemaTables() {
    static bool constructed = false;
    if (!constructed) {
        constructed = true;
        baseModel::schemasPtr_ = new baseModel::schemaTable;
    }
}

void baseModel::destroySchemaTables() {
    if (baseModel::schemasPtr_) {
        delete baseModel::schemasPtr_;
        baseModel::schemasPtr_ = nullptr;
    }
}

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

