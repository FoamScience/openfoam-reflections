#include "OpenFOAMModel.H"
#include "childOpenFOAMModel.H"
#include "reflectDictionary.H"

REFL_AUTO(template((typename Type), (Foam::Reflect::wrapAbstract<Foam::OpenFOAMModel<Type>>)),
        field(name_),
        field(m_),
        field(dir_),
        field(ht_),
        field(ndc_))

REFL_AUTO(template((typename Type), (Foam::Reflect::wrapImpl<Foam::childOpenFOAMModel<Type>>)),
        field(list_))

namespace Foam {
    defineSchemaTableTemplated(Reflect::wrapAbstract, OpenFOAMModel<vector>);
    defineSchemaTableTemplated(Reflect::wrapAbstract, OpenFOAMModel<tensor>);
    defineSchemaTableTemplated(Reflect::wrapAbstract, childOpenFOAMModel<vector>);
    defineSchemaTableTemplated(Reflect::wrapAbstract, childOpenFOAMModel<tensor>);
    wrapReflectImplementedModel(OpenFOAMModel, childOpenFOAMModel, vector);
    wrapReflectImplementedModel(OpenFOAMModel, childOpenFOAMModel, tensor);
}
