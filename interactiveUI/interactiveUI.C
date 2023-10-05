#include "dictionary.H"
#include "refl.H"
#include "uiConcepts.H"
#include "uiBuilder.H"
#include "baseModel.H"
#include <string>
#include <type_traits>

using namespace Foam;

template<class T, class B = T>
static dictionary reflectAndBuildDictionary() {
    dictionary dict;
    using pureName = std::remove_cvref_t<T>;
    using pureBName = std::remove_cvref_t<B>;
    constexpr auto type = refl::reflect<pureName>();
    constexpr bool isRTSManagedBase = ui::DictionaryRTSModelBase<pureName>;
    constexpr bool isRTSManagedImpl = ui::DictionaryRTSModelImpl<pureName, pureBName>;
    constexpr bool isAbstractModel = std::is_abstract_v<pureName>;
    constexpr bool hasSchema = ui::SelfReflectableModel<pureName>;
    Info << "Generating schema dictionary for " << word(type.name.str()) << endl;
    Info << "---- A base for RTS models? -> " << isRTSManagedBase << endl;
    Info << "---- An implementation for " << ui::builder<pureName>::demangle(typeid(pureName).name())
         << " RTS models ? -> " << isRTSManagedImpl << endl;
    Info << "---- Is abstract? -> " << isAbstractModel << endl;
    Info << "---- Can self-reflect? -> " << hasSchema << endl;
    if constexpr (hasSchema) {
        dict = ui::builder<pureName>::schema();
    }
    return dict;
}


int main (int argc, char *argv[]) {
    mff::baseModel::debug = 1;
    Info << "Choose a Model from available baseModels:" << endl;
    Info << mff::baseModel::schemasPtr_->toc() << endl;
    word modelType;
    std::getline(std::cin, modelType);
    Info << mff::baseModel::schema(modelType) << endl;
    //Info << "--------------------------------------" << endl;
    //Info << "The baseModel is reflected as:" << endl;
    //Info << reflectAndBuildDictionary<mff::baseModel>() << endl;   
    //Info << "--------------------------------------" << endl;
    //Info << reflectAndBuildDictionary<mff::childModel, mff::baseModel>() << endl;   
    //Info << "--------------------------------------" << endl;
    return 0;
}
